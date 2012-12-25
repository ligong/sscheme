#ifndef SSCHEME_SRC_BIGNUM_H_
#define SSCHEME_SRC_BIGNUM_H_

class BigNum
{

 public:

  const int kBase = 256;
  
  BigNum(): bytes_(NULL),n_(0) { }

  BigNum operator+(BigNum x, BigNum y);
  BigNum operator-(BigNum x, BigNum y);
  BigNum operator*(BigNum x, BigNum y);
  BigNum operator/(BigNum x, BigNum y);
  BigNum operator%(BigNum x, BigNum y);

 protected:
  
  unsigned char* bytes_;
  int n_; // bytes size

  int Add(int n, char* x, char* y, int carry, char* z); 
  int Sub(int n, char* x, char* y, int borrow, char* z);
  int Mul(char* x, int nx, char* y, int ny, char* z, int nz);
  //  int Div(char* x
  
};

#endif
