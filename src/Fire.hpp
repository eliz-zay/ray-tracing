#ifndef FIRE
#define FIRE

#include "vec/vec3.cpp"

class Fire: public Object {
    private:
        vec3 center;
        float radius;

    public:
        Fire(vec3 center, float radius);
        
        bool intersection(vec3 origin, vec3 dir, float* distance, vec3* color, vec3* normal);
};

#endif