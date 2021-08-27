#include "Record.hpp"

Record::Record()
{
    hit = false;
}

void Record::over_write(float new_t, vec3 n, vec3 inter, Material *m)
{
    if(!hit || new_t < min_t)
    {
        hit = true;
        min_t = new_t;
        normal = normalize(n);
        intersection = inter;
        material = m;
    }
}

void Record::shadow_write()
{
    hit = true;
}