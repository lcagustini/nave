// Sphere-Triangle collision from: http://realtimecollisiondetection.net/blog/?p=103
bool sphereCollidesTriangle(struct vector sphere_center, float sphere_radius, struct vector triangle0, struct vector triangle1, struct vector triangle2) {
    bool separated = false;
    float d1, d2, d3;

    triangle0 = vectorSubtract(triangle0, sphere_center);
    triangle1 = vectorSubtract(triangle1, sphere_center);
    triangle2 = vectorSubtract(triangle2, sphere_center);
    sphere_radius = sphere_radius * sphere_radius;

    asm(
            "fmov %[t0x], fr0\n"
            "fmov %[t1x], fr3\n"
            "fsub fr0, fr3\n"
            "fmov %[t0y], fr1\n"
            "fmov %[t1y], fr4\n"
            "fsub fr1, fr4\n"
            "fmov %[t0z], fr2\n"
            "fmov %[t1z], fr5\n"
            "fsub fr2, fr5\n"
            "fmov %[t2x], fr6\n"
            "fsub fr0, fr6\n"
            "fmov %[t2y], fr7\n"
            "fsub fr1, fr7\n"
            "fmov %[t2z], fr8\n"
            "fsub fr2, fr8\n"
            // a = t1 - t0 (fr3,4,5) b = t2 - t0 (fr6,7,8)
            // cross.x (fr9)
            "fmov fr4, fr9\n"
            "fmul fr8, fr9\n"
            "fmov fr5, fr10\n"
            "fmul fr7, fr10\n"
            "fsub fr10, fr9\n"
            // cross.y (fr10)
            "fmov fr5, fr10\n"
            "fmul fr6, fr10\n"
            "fmov fr3, fr11\n"
            "fmul fr8, fr11\n"
            "fsub fr11, fr10\n"
            // cross.z (fr11)
            "fmov fr3, fr11\n"
            "fmul fr7, fr11\n"
            "fmov fr4, fr12\n"
            "fmul fr6, fr12\n"
            "fsub fr12, fr11\n"
            // d = dot(t0, cross) (fr8)
            "fldi0 fr3\n"
            "fmov fr9, fr4\n"
            "fmov fr10, fr5\n"
            "fmov fr11, fr6\n"
            "fldi0 fr7\n"
            "fipr fv4, fv0\n"
            "fmov fr3, fr8\n"
            // e = dot(cross, cross) (fr3)
            "fmov fr9, fr0\n"
            "fmov fr10, fr1\n"
            "fmov fr11, fr2\n"
            "fldi0 fr3\n"
            "fipr fv4, fv0\n"
            // d*d (fr8)
            "fmul fr8, fr8\n"
            // sphere_radius*e (fr3)
            "fmov %[sr], fr0\n"
            "fmul fr0, fr3\n"
            // d*d > sr*e
            "fcmp/gt fr3, fr8\n"
            "movt r0\n"
            "or r0, %[sep]\n"
            : [sep] "+r" (separated)
            : [t0x] "m" (triangle0.x), [t0y] "m" (triangle0.y), [t0z] "m" (triangle0.z),
            [t1x] "m" (triangle1.x), [t1y] "m" (triangle1.y), [t1z] "m" (triangle1.z),
            [t2x] "m" (triangle2.x), [t2y] "m" (triangle2.y), [t2z] "m" (triangle2.z),
            [scx] "m" (sphere_center.x), [scy] "m" (sphere_center.y), [scz] "m" (sphere_center.z),
            [sr] "m" (sphere_radius)
            : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11", "fr12", "r0"
        );

    asm(
            // t0 -> fr0,1,2
            "fmov %[t0x], fr0\n"
            "fmov %[t0y], fr1\n"
            "fmov %[t0z], fr2\n"
            "fldi0 fr3\n"
            // t0 -> fr4,5,6
            "fmov fr0, fr4\n"
            "fmov fr1, fr5\n"
            "fmov fr2, fr6\n"
            "fldi0 fr7\n"
            // aa = dot(t0, t0) -> fr8
            "fipr fv4, fv0\n"
            "fmov fr3, fr8\n"
            // t0 -> fr0,1,2
            "fldi0 fr3\n"
            // t1 -> fr4,5,6
            "fmov %[t1x], fr4\n"
            "fmov %[t1y], fr5\n"
            "fmov %[t1z], fr6\n"
            // ab = dot(t0, t1) -> fr9
            "fipr fv4, fv0\n"
            "fmov fr3, fr9\n"
            // t0 -> fr0,1,2
            "fldi0 fr3\n"
            // t2 -> fr4,5,6
            "fmov %[t2x], fr4\n"
            "fmov %[t2y], fr5\n"
            "fmov %[t2z], fr6\n"
            // ac = dot(t0, t2) -> fr10
            "fipr fv4, fv0\n"
            "fmov fr3, fr10\n"
            // t2 -> fr0,1,2
            "fmov fr4, fr0\n"
            "fmov fr5, fr1\n"
            "fmov fr6, fr2\n"
            "fldi0 fr3\n"
            // t2 -> fr4,5,6
            // cc = dot(t2, t2) -> fr11
            "fipr fv4, fv0\n"
            "fmov fr3, fr11\n"
            // t1 -> fr0,1,2
            "fmov %[t1x], fr0\n"
            "fmov %[t1y], fr1\n"
            "fmov %[t1z], fr2\n"
            "fldi0 fr3\n"
            // t2 -> fr4,5,6
            // bc = dot(t1, t2) -> fr12
            "fipr fv4, fv0\n"
            "fmov fr3, fr12\n"
            // t1 -> fr0,1,2
            "fldi0 fr3\n"
            // t1 -> fr4,5,6
            "fmov fr0, fr4\n"
            "fmov fr1, fr5\n"
            "fmov fr2, fr6\n"
            // bb = dot(t1, t1) -> fr13
            "fipr fv4, fv0\n"
            "fmov fr3, fr13\n"
            // sphere_radius -> fr14
            "fmov %[sr], fr14\n"
            // aa > sr
            "fcmp/gt fr14, fr8\n"
            "movt r0\n"
            // ab > aa
            "fcmp/gt fr8, fr9\n"
            "movt r1\n"
            "and r1, r0\n"
            // ac > aa
            "fcmp/gt fr8, fr10\n"
            "movt r1\n"
            "and r1, r0\n"
            "or r0, %[sep]\n"
            // bb > sr
            "fcmp/gt fr14, fr13\n"
            "movt r0\n"
            // ab > bb
            "fcmp/gt fr13, fr9\n"
            "movt r1\n"
            "and r1, r0\n"
            // bc > bb
            "fcmp/gt fr13, fr12\n"
            "movt r1\n"
            "and r1, r0\n"
            "or r0, %[sep]\n"
            // cc > sr
            "fcmp/gt fr14, fr11\n"
            "movt r0\n"
            // ac > cc
            "fcmp/gt fr11, fr10\n"
            "movt r1\n"
            "and r1, r0\n"
            // bc > cc
            "fcmp/gt fr11, fr12\n"
            "movt r1\n"
            "and r1, r0\n"
            "or r0, %[sep]\n"
            // d1 = ab - aa
            "fsub fr8, fr9\n"
            "fmov fr9, %[d1]\n"
            // d2 = bc - bb
            "fsub fr13, fr12\n"
            "fmov fr12, %[d2]\n"
            // d3 = ac - cc
            "fsub fr11, fr10\n"
            "fmov fr10, %[d3]\n"
            : [sep] "+r" (separated), [d1] "=f" (d1), [d2] "=m" (d2), [d3] "=m" (d3)
            : [t0x] "m" (triangle0.x), [t0y] "m" (triangle0.y), [t0z] "m" (triangle0.z),
            [t1x] "m" (triangle1.x), [t1y] "m" (triangle1.y), [t1z] "m" (triangle1.z),
            [t2x] "m" (triangle2.x), [t2y] "m" (triangle2.y), [t2z] "m" (triangle2.z),
            [sr] "m" (sphere_radius)
            : "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11", "fr12", "fr13", "fr14", "r0", "r1"
        );

    {
        struct vector AB = vectorSubtract(triangle1, triangle0);
        float e1 = vectorDot(AB, AB);

        struct vector Q1 = vectorSubtract(vectorScale(e1, triangle0), vectorScale(d1, AB));
        struct vector QC = vectorSubtract(vectorScale(e1, triangle2), Q1);
        separated |= (vectorDot(Q1, Q1) > sphere_radius * e1 * e1) && (vectorDot(Q1, QC) > 0);
    }

    {
        struct vector BC = vectorSubtract(triangle2, triangle1);
        float e2 = vectorDot(BC, BC);

        struct vector Q2 = vectorSubtract(vectorScale(e2, triangle1), vectorScale(d2, BC));
        struct vector QA = vectorSubtract(vectorScale(e2, triangle0), Q2);
        separated |= (vectorDot(Q2, Q2) > sphere_radius * e2 * e2) && (vectorDot(Q2, QA) > 0);
    }

    {
        struct vector CA = vectorSubtract(triangle0, triangle2);
        float e3 = vectorDot(CA, CA);

        struct vector Q3 = vectorSubtract(vectorScale(e3, triangle2), vectorScale(d3, CA));
        struct vector QB = vectorSubtract(vectorScale(e3, triangle1), Q3);
        separated |= (vectorDot(Q3, Q3) > sphere_radius * e3 * e3) && (vectorDot(Q3, QB) > 0);
    }

    return !separated;
}

bool sphereCollidesSphere(struct vector sphere1_center, float sphere1_r, struct vector sphere2_center, float sphere2_r) {
    float distance = vectorLenSquared(vectorSubtract(sphere1_center, sphere2_center));
    float hit_distance = (sphere1_r + sphere2_r);
    return distance < hit_distance*hit_distance;
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool rayIntersectsTriangle(struct vector rayOrigin, struct vector rayVector, struct vector vertex0, struct vector vertex1, struct vector vertex2, struct vector *outIntersectionPoint) {
    const float EPSILON = 0.0000001;
    struct vector edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vectorSubtract(vertex1, vertex0);
    edge2 = vectorSubtract(vertex2, vertex0);
    h = vectorCross(rayVector, edge2);
    a = vectorDot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;
    f = 1/a;
    s = vectorSubtract(rayOrigin, vertex0);
    u = f * (vectorDot(s, h));
    if (u < 0.0 || u > 1.0)
        return false;
    q = vectorCross(s, edge1);
    v = f * vectorDot(rayVector, q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * vectorDot(edge2, q);
    if (t > EPSILON) { // ray intersection
        *outIntersectionPoint = vectorAdd(rayOrigin, vectorScale(t, rayVector));
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

