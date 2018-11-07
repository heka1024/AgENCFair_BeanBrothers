#ifndef __SUN_H_
#define __SUN_H_

#define pi    3.14159265358979323846
#define twopi (2*pi)
#define rad   (pi/180)
#define dEarthMeanRadius     6371.01	// In km
#define dAstronomicalUnit    149597890	// In km

class Sun {
	public:
    void setLoc(double longitude, double latitude);
    void setTime(int year, int month, int date,
      double hour, double minute, double second);
    void calcPos();
    double azimuth, altitude;
  private:
    int yr, mon, day;
    double hr, min, sec;
    double lon, lat;
};
#endif
