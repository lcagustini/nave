// Sphere-Triangle collision from: http://realtimecollisiondetection.net/blog/?p=103
bool sphereCollidesTriangle(struct vector sphere_center, float sphere_radius, struct vector triangle0, struct vector triangle1, struct vector triangle2) {
    struct vector A = vectorSubtract(triangle0, sphere_center);
    struct vector B = vectorSubtract(triangle1, sphere_center);
    struct vector C = vectorSubtract(triangle2, sphere_center);
    float rr = sphere_radius * sphere_radius;
    struct vector V = vectorCross(vectorSubtract(B, A), vectorSubtract(C, A));
    float d = vectorDot(A, V);
    float e = vectorDot(V, V);

    bool sep1 = d*d > rr*e;

    float aa = vectorDot(A, A);
    float ab = vectorDot(A, B);
    float ac = vectorDot(A, C);
    float bb = vectorDot(B, B);
    float bc = vectorDot(B, C);
    float cc = vectorDot(C, C);

    bool sep2 = (aa > rr) && (ab > aa) && (ac > aa);
    bool sep3 = (bb > rr) && (ab > bb) && (bc > bb);
    bool sep4 = (cc > rr) && (ac > cc) && (bc > cc);

    struct vector AB = vectorSubtract(B, A);
    struct vector BC = vectorSubtract(C, B);
    struct vector CA = vectorSubtract(A, C);

    float d1 = ab - aa;
    float d2 = bc - bb;
    float d3 = ac - cc;

    float e1 = vectorDot(AB, AB);
    float e2 = vectorDot(BC, BC);
    float e3 = vectorDot(CA, CA);

    struct vector Q1 = vectorSubtract(vectorScale(e1, A), vectorScale(d1, AB));
    struct vector Q2 = vectorSubtract(vectorScale(e2, B), vectorScale(d2, BC));
    struct vector Q3 = vectorSubtract(vectorScale(e3, C), vectorScale(d3, CA));
    struct vector QC = vectorSubtract(vectorScale(e1, C), Q1);
    struct vector QA = vectorSubtract(vectorScale(e2, A), Q2);
    struct vector QB = vectorSubtract(vectorScale(e3, B), Q3);

    bool sep5 = (vectorDot(Q1, Q1) > rr * e1 * e1) && (vectorDot(Q1, QC) > 0);
    bool sep6 = (vectorDot(Q2, Q2) > rr * e2 * e2) && (vectorDot(Q2, QA) > 0);
    bool sep7 = (vectorDot(Q3, Q3) > rr * e3 * e3) && (vectorDot(Q3, QB) > 0);

    bool separated = sep1 || sep2 || sep3 || sep4 || sep5 || sep6 || sep7;

    return !separated;
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool rayIntersectsTriangle(struct vector rayOrigin, struct vector rayVector, struct face *inTriangle, struct vector *outIntersectionPoint) {
    const float EPSILON = 0.0000001;
    struct vector vertex0 = vectorScale(MAP_SCALE, cur_map.model.vertices[inTriangle->vertices[0]]);
    struct vector vertex1 = vectorScale(MAP_SCALE, cur_map.model.vertices[inTriangle->vertices[1]]);
    struct vector vertex2 = vectorScale(MAP_SCALE, cur_map.model.vertices[inTriangle->vertices[2]]);
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

void collidesWithMap(int id, struct vector *max_z) {
    struct vector next_pos = vectorAdd(entities[id].pos, entities[id].dir);
    struct vector rotation_points[4] = {{-200, -200, -200}, {-200, -200, -200}, {-200, -200, -200}, {-200, -200, -200}};
    struct vector rotation_normals[4] = {{-200, -200, -200}, {-200, -200, -200}, {-200, -200, -200}, {-200, -200, -200}};

    for (int i = 0; i < cur_map.model.num_faces; i++) {
        struct face *cur = &cur_map.model.faces[i];

        struct vector vertex0 = vectorScale(MAP_SCALE, cur_map.model.vertices[cur->vertices[0]]);
        struct vector vertex1 = vectorScale(MAP_SCALE, cur_map.model.vertices[cur->vertices[1]]);
        struct vector vertex2 = vectorScale(MAP_SCALE, cur_map.model.vertices[cur->vertices[2]]);

        struct vector v1 = vectorSubtract(vertex2, vertex0);
        struct vector v2 = vectorSubtract(vertex1, vertex0);
        struct vector normal = vectorCross(v1, v2);
        vectorNormalize(&normal);
        if (normal.z < 0) {
            normal = vectorScale(-1, normal);
        }

        struct vector up = {0, 0, 1};

        float cosine = vectorDot(normal, up);

        float angle = acos(cosine) * 180 / M_PI;

        // walls
        if (angle > 60 &&
                (vertex0.z > next_pos.z + 0.03 ||
                 vertex1.z > next_pos.z + 0.03 ||
                 vertex2.z > next_pos.z + 0.03)) {
            bool collides = sphereCollidesTriangle(next_pos, entities[id].hit_radius, vertex0, vertex1, vertex2);

            if (collides) {
                struct vector v = vectorSubtract(entities[id].pos, vertex0);
                float d = vectorDot(v, normal);
                struct vector collision = vectorScale(d, normal);

                struct vector reaction_v = vectorAdd(entities[id].dir, collision);
                entities[id].dir.x = reaction_v.x * entities[id].dir.x > 0 ? reaction_v.x : 0;
                entities[id].dir.y = reaction_v.y * entities[id].dir.y > 0 ? reaction_v.y : 0;
                entities[id].dir.z = reaction_v.z * entities[id].dir.z > 0 ? reaction_v.z : 0;
            }
        }

        // floors
        else {
            struct vector sky[4] = {
                {entities[id].pos.x + entities[id].hit_radius, entities[id].pos.y, 200},
                {entities[id].pos.x - entities[id].hit_radius, entities[id].pos.y, 200},
                {entities[id].pos.x, entities[id].pos.y + entities[id].hit_radius, 200},
                {entities[id].pos.x, entities[id].pos.y - entities[id].hit_radius, 200}
            };

            struct vector ground = {0, 0, -1};

            struct vector intersect_v;
            for (int j = 0; j < 4; j++) {
                bool intersect = rayIntersectsTriangle(sky[j], ground, cur, &intersect_v);
                if (intersect) {
                    // only false if slime is out of bounds
                    if (rotation_points[j].z < intersect_v.z) {
                        rotation_points[j] = intersect_v;
                        rotation_normals[j] = normal;
                    }
                }
            }

            struct vector sky_center = {entities[id].pos.x, entities[id].pos.y, 200};
            bool intersect = rayIntersectsTriangle(sky_center, ground, cur, &intersect_v);
            if (intersect) {
                if (max_z->z < intersect_v.z) {
                    *max_z = intersect_v;
                }
            }
        }
    }

    struct vector normal_sum = {0};
    struct vector up = {0,0,1};

    for (int j = 0; j < 4; j++) {
        normal_sum = vectorAdd(normal_sum, rotation_normals[j]);
    }
    vectorNormalize(&normal_sum);
    entities[id].rotation = getRotationQuat(up, normal_sum);

    if (vectorLen(entities[id].dir) > entities[id].speed) {
        vectorNormalize(&entities[id].dir);
        entities[id].dir = vectorScale(entities[id].speed, entities[id].dir);
    }
}

bool sphereCollidesSphere(struct vector sphere1_center, float sphere1_r, struct vector sphere2_center, float sphere2_r) {
    float distance = vectorLenSquared(vectorSubtract(sphere1_center, sphere2_center));
    float hit_distance = (sphere1_r + sphere2_r);
    return distance < hit_distance*hit_distance;
}

void collidesWithPlayer(int id) {
    struct vector next_pos = vectorAdd(entities[id].pos, entities[id].dir);

    bool collides = sphereCollidesSphere(entities[PLAYER_ID].pos, entities[PLAYER_ID].hit_radius, next_pos, entities[id].hit_radius);

    if (collides) {
        struct vector v = vectorSubtract(entities[id].pos, entities[PLAYER_ID].pos);
        vectorNormalize(&v);
        v = vectorScale(entities[id].speed, v);

        entities[id].dir = vectorAdd(entities[id].dir, v);
    }
}
