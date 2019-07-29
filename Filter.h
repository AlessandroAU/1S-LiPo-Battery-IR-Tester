//Low pass bessel filter order=1 alpha1=0.1 
class  FilterBeLp1
{
  public:
    FilterBeLp1()
    {
      v[0]=0.0;
    }
  private:
    float v[2];
  public:
    float step(float x) //class II 
    {
      v[0] = v[1];
      v[1] = (2.452372752527856026e-1 * x)
         + (0.50952544949442879485 * v[0]);
      return 
         (v[0] + v[1]);
    }
};

extern FilterBeLp1 lowpass; 
