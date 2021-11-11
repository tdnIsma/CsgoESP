#pragma once
#include "math.h"
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}



class Player
{
public:

    // be sure to put all the offsets you pad this way in a union
    union
    {
        //              Type     Name    Offset
        DEFINE_MEMBER_N(int, Health, 0x100);
        DEFINE_MEMBER_N(int, Team, 0xF4);
        DEFINE_MEMBER_N(int, fov, 0x31F8);
        DEFINE_MEMBER_N(bool, isDormant, 0xED);
        DEFINE_MEMBER_N(Vector3, vecOrigin, 0x138);
        DEFINE_MEMBER_N(int, Flags, 0x104);
        DEFINE_MEMBER_N(int, ForceJump,0x524BF4C);
        DEFINE_MEMBER_N(int, BoneMatrix, 0x26A8);
    };

};


class EntListObj
{
public:
    Player* ent;
    char padding[12];
};



class Entity {
public:
    EntListObj entListObjs[32];
};
