#include "main.h"

/**
 * Converts degrees to radians.
 * 
 * @param degrees The value in degrees to convert.
 * @return The converted value in radians.
 */
float degrees_to_radians(float degrees)
{
    return degrees * PI / 180.0;
}


float haversine(float lat1, float lon1, float lat2, float lon2)
{
    float dlat, dlon, a, c, d;

    // Convert latitude and longitude to decimal degrees format
    lat1 = GPStoDeg(lat1);
    lon1 = GPStoDeg(lon1);
    lat2 = GPStoDeg(lat2);
    lon2 = GPStoDeg(lon2);

    dlat = degrees_to_radians(lat2 - lat1);
    dlon = degrees_to_radians(lon2 - lon1);

    a = sin(dlat/2) * sin(dlat/2) + cos(degrees_to_radians(lat1)) * cos(degrees_to_radians(lat2)) * sin(dlon/2) * sin(dlon/2);
    c = 2 * atan2(sqrt(a), sqrt(1-a));
    d = EARTH_RADIUS * c;

    return d;
}
