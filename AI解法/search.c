#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>
#define SIZE 292
#define OFF_BLAST3 0x3d
#define OFF_ESI 0x07
#define OFF_ENTRY 0x18
#define OFF_VADDR 0x48
#define OFF_IV (SIZE-12)
static uint32_t K[64];
static const uint8_t S[64]={
 7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
 5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
 4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
 6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21};
static void init_K(void){for(int i=0;i<64;i++){double s=__builtin_sin((double)(i+1));if(s<0)s=-s;K[i]=(uint32_t)(s*4294967296.0);}}
static inline uint32_t rotl(uint32_t x,int n){return (x<<n)|(x>>(32-n));}
static void compress(uint32_t st[4], const uint8_t *block){
  uint32_t A=st[0],B=st[1],C=st[2],D=st[3],M[16];
  memcpy(M,block,64);
  for(int i=0;i<64;i++){
    uint32_t F,g;
    if(i<16){F=(B&C)|(~B&D);g=i;}
    else if(i<32){F=(D&B)|(~D&C);g=(5*i+1)&15;}
    else if(i<48){F=B^C^D;g=(3*i+5)&15;}
    else{F=C^(B|~D);g=(7*i)&15;}
    F+=A+K[i]+M[g]; A=D; D=C; C=B; B+=rotl(F,S[i]);
  }
  st[0]+=A;st[1]+=B;st[2]+=C;st[3]+=D;
}
static int hex_ok(const uint8_t *dig){
  for(int i=0;i<16;i++) if((dig[i]&0xf)==0) return 0;
  return 1;
}
int main(int argc,char **argv){
  const char *path=argc>1?argv[1]:"selfmd5";
  int fd=open(path,O_RDWR); if(fd<0){perror(path);return 1;}
  uint8_t templ[SIZE]; if(read(fd,templ,SIZE)!=SIZE){perror("read");return 1;}
  init_K();
  struct timespec t0; clock_gettime(CLOCK_MONOTONIC,&t0);
  volatile int found=0;
  uint8_t hitbuf[SIZE];
  uint32_t hit_b=0,hit_org=0,hit_iv[3];
  uint8_t hit_dig[16];
  uint64_t grand=0;
  printf("search IV3==0 + hex_ok size=%d thr=%d\n",SIZE,omp_get_max_threads()); fflush(stdout);
  #pragma omp parallel
  {
    uint8_t data[SIZE];
    uint64_t local=0, last_report=0;
    #pragma omp for schedule(static)
    for(uint32_t b=0;b<0x1000000u;b++){
      if(found) continue;
      for(uint32_t p=0;p<8192u;p++){ /* more orgs: 13 bits */
        uint32_t org=0x10000u+(p<<12);
        if(org>0x7fff0000u) break;
        memcpy(data,templ,SIZE);
        data[OFF_BLAST3]=(uint8_t)b;
        data[OFF_BLAST3+1]=(uint8_t)(b>>8);
        data[OFF_BLAST3+2]=(uint8_t)(b>>16);
        uint32_t esi=org+(SIZE-12);
        memcpy(data+OFF_ESI,&esi,4);
        uint64_t e=org+6,v=org;
        memcpy(data+OFF_ENTRY,&e,8);
        memcpy(data+OFF_VADDR,&v,8);
        uint32_t st[4]={0x67452301u,0xEFCDAB89u,0x98BADCFEu,0x10325476u};
        compress(st,data); compress(st,data+64);
        compress(st,data+128); compress(st,data+192);
        local++;
        if(__builtin_expect(st[3]!=0,1)) continue;
        memcpy(data+OFF_IV,st,12);
        uint32_t hs[4]={0x67452301u,0xEFCDAB89u,0x98BADCFEu,0x10325476u};
        uint8_t pad[64];
        compress(hs,data); compress(hs,data+64);
        compress(hs,data+128); compress(hs,data+192);
        memcpy(pad,data+256,SIZE-256); pad[SIZE-256]=0x80;
        memset(pad+(SIZE-256)+1,0,64-(SIZE-256)-1-8);
        uint64_t bits=(uint64_t)SIZE*8; memcpy(pad+56,&bits,8);
        compress(hs,pad);
        if(!hex_ok((uint8_t*)hs)) continue;
        #pragma omp critical
        if(!found){
          found=1;
          memcpy(hitbuf,data,SIZE);
          hit_b=b; hit_org=org;
          hit_iv[0]=st[0]; hit_iv[1]=st[1]; hit_iv[2]=st[2];
          memcpy(hit_dig,hs,16);
        }
      }
      if(local-last_report >= (1ull<<22)){
        #pragma omp atomic
        grand += (local-last_report);
        last_report=local;
        if(omp_get_thread_num()==0 && !found){
          struct timespec t1; clock_gettime(CLOCK_MONOTONIC,&t1);
          double sec=(t1.tv_sec-t0.tv_sec)+(t1.tv_nsec-t0.tv_nsec)*1e-9;
          uint64_t g;
          #pragma omp atomic read
          g=grand;
          if(sec>0.2) printf("%.1fs tried=%llu (%.2fM/s)\n",sec,(unsigned long long)g,g/sec/1e6),fflush(stdout);
        }
      }
    }
    #pragma omp atomic
    grand += (local-last_report);
  }
  if(!found){printf("no hit tried=%llu\n",(unsigned long long)grand);close(fd);return 2;}
  char hex[33]; const char *x="0123456789abcdef";
  for(int i=0;i<16;i++){hex[i*2]=x[hit_dig[i]>>4];hex[i*2+1]=x[hit_dig[i]&15];} hex[32]=0;
  printf("HIT b=0x%x org=0x%x IV=%08x %08x %08x md5=%s\n",
         hit_b,hit_org,hit_iv[0],hit_iv[1],hit_iv[2],hex);
  lseek(fd,0,SEEK_SET); write(fd,hitbuf,SIZE); fsync(fd);
  printf("WROTE\n"); close(fd); return 0;
}
