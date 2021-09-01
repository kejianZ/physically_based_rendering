#include "Record.hpp"
#include <iostream>

Record::Record()
{
    hit = false;
}

bool Record::over_write(float new_t, vec4 n, vec4 inter, Material *m)
{
    if(!hit || new_t < min_t)
    {
        hit = true;
        min_t = new_t;
        normal = n;
        intersection = inter;
        material = m;
        return true;
    }
    return false;
}

void Record::shadow_write()
{
    hit = true;
}