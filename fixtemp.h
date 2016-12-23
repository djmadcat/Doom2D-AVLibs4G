// Requires generic.h

int name3(_fixed,B,_mul) (int a,int b);
#pragma aux name3(_fixed,B,_mul) = \
  "imul ebx" \
  SHRDB \
  parm [eax] [ebx] \
  modify [edx] \
  value [eax]

int name3(_fixed,B,_div) (int a,int b);
#pragma aux name3(_fixed,B,_div) = \
  "mov edx,eax" \
  "sar edx,31" \
  SHLDB \
  "idiv ebx" \
  parm [eax] [ebx] \
  modify [edx] \
  value [eax]

#define fixb name2(fix,B)

class fixb : public __fixbase {
public:
//  int v;
  fixb() {}
  fixb(const fixb& a) {v=a.v;}
  fixb(int a) {v=a<<B;}
  fixb(unsigned a) {v=a<<B;}
  fixb(double a) {v=int((1<<B)*a);}
  fixb(float a) {v=int((1<<B)*a);}

  operator int() {return v>>B;}
  operator unsigned() {return v>>B;}
  operator double() {return v/double(1<<B);}
  operator float() {return v/float(1<<B);}

  fixb operator -() {fixb f;f.v=-v;return f;}
  fixb operator +() {return *this;}

  fixb operator +(fixb a) {fixb f;f.v=v+a.v;return f;}
  fixb operator -(fixb a) {fixb f;f.v=v-a.v;return f;}
  fixb operator *(fixb a) {fixb f;f.v=name3(_fixed,B,_mul) (v,a.v);return f;}
  fixb operator /(fixb a) {fixb f;f.v=name3(_fixed,B,_div) (v,a.v);return f;}
  fixb operator <<(int a) {fixb f;f.v=v<<a;return f;}
  fixb operator >>(int a) {fixb f;f.v=v>>a;return f;}

  fixb& operator =(fixb a) {v=a.v;return *this;}
  fixb& operator +=(fixb a) {v+=a.v;return *this;}
  fixb& operator -=(fixb a) {v-=a.v;return *this;}
  fixb& operator *=(fixb a) {v=name3(_fixed,B,_mul) (v,a.v);return *this;}
  fixb& operator /=(fixb a) {v=name3(_fixed,B,_div) (v,a.v);return *this;}
  fixb& operator <<=(int a) {v<<=a;return *this;}
  fixb& operator >>=(int a) {v>>=a;return *this;}

  int operator ==(fixb a) {return v==a.v;}
  int operator !=(fixb a) {return v!=a.v;}
  int operator < (fixb a) {return v< a.v;}
  int operator <=(fixb a) {return v<=a.v;}
  int operator > (fixb a) {return v> a.v;}
  int operator >=(fixb a) {return v>=a.v;}
};

inline fixb operator +(fixb a,int b) {return a+fixb(b);}
inline fixb operator +(fixb a,unsigned b) {return a+fixb(b);}
inline fixb operator +(fixb a,float b) {return a+fixb(b);}
inline fixb operator +(fixb a,double b) {return a+fixb(b);}
inline fixb operator +(int a,fixb b) {return fixb(a)+b;}
inline fixb operator +(unsigned a,fixb b) {return fixb(a)+b;}
inline fixb operator +(float a,fixb b) {return fixb(a)+b;}
inline fixb operator +(double a,fixb b) {return fixb(a)+b;}

inline fixb operator -(fixb a,int b) {return a-fixb(b);}
inline fixb operator -(fixb a,unsigned b) {return a-fixb(b);}
inline fixb operator -(fixb a,float b) {return a-fixb(b);}
inline fixb operator -(fixb a,double b) {return a-fixb(b);}
inline fixb operator -(int a,fixb b) {return fixb(a)-b;}
inline fixb operator -(unsigned a,fixb b) {return fixb(a)-b;}
inline fixb operator -(float a,fixb b) {return fixb(a)-b;}
inline fixb operator -(double a,fixb b) {return fixb(a)-b;}

inline fixb operator *(fixb a,int b) {fixb f;f.v=a.v*b;return f;}
inline fixb operator *(fixb a,unsigned b) {fixb f;f.v=a.v*b;return f;}
inline fixb operator *(fixb a,float b) {return a*fixb(b);}
inline fixb operator *(fixb a,double b) {return a*fixb(b);}
inline fixb operator *(int a,fixb b) {fixb f;f.v=a*b.v;return f;}
inline fixb operator *(unsigned a,fixb b) {fixb f;f.v=a*b.v;return f;}
inline fixb operator *(float a,fixb b) {return fixb(a)*b;}
inline fixb operator *(double a,fixb b) {return fixb(a)*b;}

inline fixb operator /(fixb a,int b) {fixb f;f.v=a.v/b;return f;}
inline fixb operator /(fixb a,unsigned b) {fixb f;f.v=a.v/b;return f;}
inline fixb operator /(fixb a,float b) {return a/fixb(b);}
inline fixb operator /(fixb a,double b) {return a/fixb(b);}
inline fixb operator /(int a,fixb b) {return fixb(a)/b;}
inline fixb operator /(unsigned a,fixb b) {return fixb(a)/b;}
inline fixb operator /(float a,fixb b) {return fixb(a)/b;}
inline fixb operator /(double a,fixb b) {return fixb(a)/b;}

inline fixb operator ==(fixb a,int b) {return a==fixb(b);}
inline fixb operator ==(fixb a,unsigned b) {return a==fixb(b);}
inline fixb operator ==(fixb a,float b) {return a==fixb(b);}
inline fixb operator ==(fixb a,double b) {return a==fixb(b);}
inline fixb operator ==(int a,fixb b) {return fixb(a)==b;}
inline fixb operator ==(unsigned a,fixb b) {return fixb(a)==b;}
inline fixb operator ==(float a,fixb b) {return fixb(a)==b;}
inline fixb operator ==(double a,fixb b) {return fixb(a)==b;}

inline fixb operator !=(fixb a,int b) {return a!=fixb(b);}
inline fixb operator !=(fixb a,unsigned b) {return a!=fixb(b);}
inline fixb operator !=(fixb a,float b) {return a!=fixb(b);}
inline fixb operator !=(fixb a,double b) {return a!=fixb(b);}
inline fixb operator !=(int a,fixb b) {return fixb(a)!=b;}
inline fixb operator !=(unsigned a,fixb b) {return fixb(a)!=b;}
inline fixb operator !=(float a,fixb b) {return fixb(a)!=b;}
inline fixb operator !=(double a,fixb b) {return fixb(a)!=b;}

inline fixb operator < (fixb a,int b) {return a< fixb(b);}
inline fixb operator < (fixb a,unsigned b) {return a< fixb(b);}
inline fixb operator < (fixb a,float b) {return a< fixb(b);}
inline fixb operator < (fixb a,double b) {return a< fixb(b);}
inline fixb operator < (int a,fixb b) {return fixb(a)< b;}
inline fixb operator < (unsigned a,fixb b) {return fixb(a)< b;}
inline fixb operator < (float a,fixb b) {return fixb(a)< b;}
inline fixb operator < (double a,fixb b) {return fixb(a)< b;}

inline fixb operator <=(fixb a,int b) {return a<=fixb(b);}
inline fixb operator <=(fixb a,unsigned b) {return a<=fixb(b);}
inline fixb operator <=(fixb a,float b) {return a<=fixb(b);}
inline fixb operator <=(fixb a,double b) {return a<=fixb(b);}
inline fixb operator <=(int a,fixb b) {return fixb(a)<=b;}
inline fixb operator <=(unsigned a,fixb b) {return fixb(a)<=b;}
inline fixb operator <=(float a,fixb b) {return fixb(a)<=b;}
inline fixb operator <=(double a,fixb b) {return fixb(a)<=b;}

inline fixb operator > (fixb a,int b) {return a> fixb(b);}
inline fixb operator > (fixb a,unsigned b) {return a> fixb(b);}
inline fixb operator > (fixb a,float b) {return a> fixb(b);}
inline fixb operator > (fixb a,double b) {return a> fixb(b);}
inline fixb operator > (int a,fixb b) {return fixb(a)> b;}
inline fixb operator > (unsigned a,fixb b) {return fixb(a)> b;}
inline fixb operator > (float a,fixb b) {return fixb(a)> b;}
inline fixb operator > (double a,fixb b) {return fixb(a)> b;}

inline fixb operator >=(fixb a,int b) {return a>=fixb(b);}
inline fixb operator >=(fixb a,unsigned b) {return a>=fixb(b);}
inline fixb operator >=(fixb a,float b) {return a>=fixb(b);}
inline fixb operator >=(fixb a,double b) {return a>=fixb(b);}
inline fixb operator >=(int a,fixb b) {return fixb(a)>=b;}
inline fixb operator >=(unsigned a,fixb b) {return fixb(a)>=b;}
inline fixb operator >=(float a,fixb b) {return fixb(a)>=b;}
inline fixb operator >=(double a,fixb b) {return fixb(a)>=b;}

inline fixb modfix(fixb a,fixb *i) {
  if(i) i->v=a.v&(0xFFFFFFFF<<B);
  fixb f; f.v=(a.v&(0xFFFFFFFFU>>(32-B))) | ((a.v&0x80000000)?(0xFFFFFFFF<<B):0);
  return f;
}

inline fixb fixabs(fixb a) {fixb f;f.v=(a.v<0)?-a.v:a.v;return f;}
inline fixb fixsqrt(fixb a) {
  if(a<=0) return a;
  fixb f=a; for(int i=0;i<B;++i) f=(a/f+f)>>1;
  return f;
}

inline fixb fixfloor(fixb a) {
  fixb f; f.v=a.v&(0xFFFFFFFF<<B);
  return f;
}

inline fixb fixceil(fixb a) {
  fixb f; f.v=(a.v+(0xFFFFFFFFU>>(32-B)))&(0xFFFFFFFF<<B);
  return f;
}

#undef fixb

#undef B
#undef SHRDB
#undef SHLDB
