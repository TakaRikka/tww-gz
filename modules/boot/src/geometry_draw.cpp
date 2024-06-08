#include "geometry_draw.h"
#include "global_data.h"
#include "libtww/include/d/com/d_com_inf_game.h"
#include "libtww/include/dolphin/gx/gx.h"
#include "libtww/include/JSystem/J3DGraphBase/J3DSys.h"
#include "libtww/include/m_Do/m_Do_printf.h"
#include "libtww/include/MSL_C/math.h"

#include "font.h"

#define DRAW_PACKET_MAX 1000  // max amount of draw packets allowed to be drawn at a time

u32 l_drawPacketListNum;
static J3DPacket* l_drawPacketList[DRAW_PACKET_MAX];

J3DPacket* dDbVw_setDrawPacketList(J3DPacket* p_packet, int buf_type) {
    if (p_packet == NULL) {
        p_packet = NULL;
    } else {
        if (l_drawPacketListNum >= DRAW_PACKET_MAX) {
            delete p_packet;
            p_packet = NULL;
        } else {
            l_drawPacketList[l_drawPacketListNum] = p_packet;
            l_drawPacketListNum++;
            J3DDrawBuffer__entryImm(j3dSys.getDrawBuffer(buf_type), p_packet, 0);
        }
    }
    return p_packet;
}

void dDbVw_deleteDrawPacketList() {
    for (u32 i = 0; i < l_drawPacketListNum; i++) {
        if (l_drawPacketList[i] != NULL) {
            delete l_drawPacketList[i];
            l_drawPacketList[i] = NULL;
        }
    }

    l_drawPacketListNum = 0;
}

//-------------------------------------------------------
//                         CUBE
//-------------------------------------------------------

void drawCube(MtxP mtx, cXyz* array, const GXColor& color) {
    GXSetArray(GX_VA_POS, array, sizeof(cXyz));
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, color);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRC_ALPHA, GX_BL_INV_SRC_ALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXLoadPosMtxImm(mtx, 0);
    GXSetCurrentMtx(0);

    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 14);
    GXPosition1x8(4);
    GXPosition1x8(6);
    GXPosition1x8(5);
    GXPosition1x8(7);
    GXPosition1x8(3);
    GXPosition1x8(6);
    GXPosition1x8(2);
    GXPosition1x8(4);
    GXPosition1x8(0);
    GXPosition1x8(5);
    GXPosition1x8(1);
    GXPosition1x8(3);
    GXPosition1x8(0);
    GXPosition1x8(2);
    GXEnd();
}

static J3DPacket__vtable_t mDoExt_cubePacket__vtable {
    (void*)nullptr,  // RTTI
    (void*)nullptr,  // pad
    (void*)&J3DPacket__isSame,
    (void*)&J3DPacket__entry,
    (void*)&mDoExt_cubePacket__draw,
    (void*)&mDoExt_cubePacket__dtor,
};

KEEP_FUNC void dDbVw_drawCubeXlu(cXyz& pos, cXyz& size, csXyz& angle, const GXColor& color) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        mDoExt_cubePacket* cube = new mDoExt_cubePacket(pos, size, angle, color);
        cube->base.vtable = &mDoExt_cubePacket__vtable;

        dDbVw_setDrawPacketList(&cube->base, 1);
    }
}

void mDoExt_cubePacket__dtor(mDoExt_cubePacket* i_this) {
    i_this->~mDoExt_cubePacket();
    delete i_this;
}

void mDoExt_cubePacket__draw(mDoExt_cubePacket* i_this) {
    static Vec points[8] = {
        {-1.0f, 1.0f, -1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
    };

    mDoMtx_stack_c::transS(i_this->mPosition.x, i_this-> mPosition.y, i_this->mPosition.z);
    mDoMtx_stack_c::XYZrotM(i_this->mAngle.x, i_this->mAngle.y, i_this->mAngle.z);
    mDoMtx_stack_c::scaleM(i_this->mSize.x, i_this->mSize.y, i_this->mSize.z);
    mDoMtx_stack_c::revConcat(j3dSys.getViewMtx());
    drawCube(mDoMtx_stack_c::get(), (cXyz*)points, i_this->mColor);
}

//-------------------------------------------------------
//                        SPHERE
//-------------------------------------------------------

static J3DPacket__vtable_t mDoExt_spherePacket__vtable {
    (void*)nullptr,  // RTTI
    (void*)nullptr,  // pad
    (void*)&J3DPacket__isSame,
    (void*)&J3DPacket__entry,
    (void*)&mDoExt_spherePacket__draw,
    (void*)&mDoExt_spherePacket__dtor,
};

KEEP_FUNC void dDbVw_drawSphereXlu(cXyz& position, f32 radius, const GXColor& color, u8 param_3) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        mDoExt_spherePacket* sph = new mDoExt_spherePacket(position, radius, color, param_3);
        sph->base.vtable = &mDoExt_spherePacket__vtable;

        dDbVw_setDrawPacketList(&sph->base, 1);
    }
}

void mDoExt_spherePacket__dtor(mDoExt_spherePacket* i_this) {
    i_this->~mDoExt_spherePacket();
    delete i_this;
}

void mDoExt_spherePacket__draw(mDoExt_spherePacket* i_this) {
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

    if (i_this->_24) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRC_ALPHA, GX_BL_INV_SRC_ALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);

    mDoMtx_stack_c::copy(j3dSys.getViewMtx());
    mDoMtx_stack_c::transM(i_this->mPosition.x, i_this->mPosition.y, i_this->mPosition.z);
    mDoMtx_stack_c::scaleM(i_this->mSize, i_this->mSize, i_this->mSize);

    GXLoadPosMtxImm(mDoMtx_stack_c::get(), 0);
    mDoMtx_stack_c::inverseTranspose();

    GXLoadNrmMtxImm(mDoMtx_stack_c::get(), 0);
    GXSetCurrentMtx(0);

    GXDrawSphere(8, 8);
}

//-------------------------------------------------------
//                       CYLINDER
//-------------------------------------------------------

static J3DPacket__vtable_t mDoExt_cylinderPacket__vtable {
    (void*)nullptr,  // RTTI
    (void*)nullptr,  // pad
    (void*)&J3DPacket__isSame,
    (void*)&J3DPacket__entry,
    (void*)&mDoExt_cylinderPacket__draw,
    (void*)&mDoExt_cylinderPacket__dtor,
};

KEEP_FUNC void dDbVw_drawCylinderXlu(cXyz& position, f32 radius, f32 height, const GXColor& color, u8 param_4) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        mDoExt_cylinderPacket* cyl = new mDoExt_cylinderPacket(position, radius, height, color, param_4);
        cyl->base.vtable = &mDoExt_cylinderPacket__vtable;

        dDbVw_setDrawPacketList(&cyl->base, 1);
    }
}

void mDoExt_cylinderPacket__dtor(mDoExt_cylinderPacket* i_this) {
    i_this->~mDoExt_cylinderPacket();
    delete i_this;
}

void mDoExt_cylinderPacket__draw(mDoExt_cylinderPacket* i_this) {
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

    if (i_this->_28) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRC_ALPHA, GX_BL_INV_SRC_ALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);

    f32 height = i_this->mHeight;

    mDoMtx_stack_c::copy(j3dSys.getViewMtx());
    mDoMtx_stack_c::transM(i_this->mPosition.x, i_this->mPosition.y + (height / 2), i_this->mPosition.z);
    mDoMtx_stack_c::scaleM(i_this->mRadius, i_this->mRadius + (height / 2), i_this->mRadius);
    mDoMtx_stack_c::XrotM(0x4000);

    GXLoadPosMtxImm(mDoMtx_stack_c::get(), 0);
    mDoMtx_stack_c::inverseTranspose();

    GXLoadNrmMtxImm(mDoMtx_stack_c::get(), 0);
    GXSetCurrentMtx(0);

    GXDrawCylinder(8);
}

//-------------------------------------------------------
//                     8 Point Cube
//-------------------------------------------------------

static J3DPacket__vtable_t mDoExt_cube8pPacket__vtable {
    (void*)nullptr,  // RTTI
    (void*)nullptr,  // pad
    (void*)&J3DPacket__isSame,
    (void*)&J3DPacket__entry,
    (void*)&mDoExt_cube8pPacket__draw,
    (void*)&mDoExt_cube8pPacket__dtor,
};

KEEP_FUNC void dDbVw_drawCube8pXlu(cXyz* points, const GXColor& color) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        mDoExt_cube8pPacket* cube = new mDoExt_cube8pPacket(points, color);
        cube->base.vtable = &mDoExt_cube8pPacket__vtable;

        dDbVw_setDrawPacketList(&cube->base, 1);
    }
}

void mDoExt_cube8pPacket__dtor(mDoExt_cube8pPacket* i_this) {
    i_this->~mDoExt_cube8pPacket();
    delete i_this;
}

void mDoExt_cube8pPacket__draw(mDoExt_cube8pPacket* i_this) {
    drawCube(j3dSys.getViewMtx(), i_this->mPoints, i_this->mColor);
}

//-------------------------------------------------------
//                       Triangle
//-------------------------------------------------------

static J3DPacket__vtable_t mDoExt_trianglePacket__vtable {
    (void*)nullptr,  // RTTI
    (void*)nullptr,  // pad
    (void*)&J3DPacket__isSame,
    (void*)&J3DPacket__entry,
    (void*)&mDoExt_trianglePacket__draw,
    (void*)&mDoExt_trianglePacket__dtor,
};

KEEP_FUNC void dDbVw_drawTriangleXlu(cXyz* points, const GXColor& color, u8 param_2) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        mDoExt_trianglePacket* tri = new mDoExt_trianglePacket(points, color, param_2);
        tri->base.vtable = &mDoExt_trianglePacket__vtable;

        dDbVw_setDrawPacketList(&tri->base, 1);
    }
}

void mDoExt_trianglePacket__dtor(mDoExt_trianglePacket* i_this) {
    i_this->~mDoExt_trianglePacket();
    delete i_this;
}

void mDoExt_trianglePacket__draw(mDoExt_trianglePacket* i_this) {
    J3DSys__reinitGX(&j3dSys);

    GXSetArray(GX_VA_POS, i_this->mPoints, sizeof(cXyz));
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
    GXLoadPosMtxImm(j3dSys.getViewMtx(), 0);
    GXSetCurrentMtx(0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetZCompLoc(GX_ENABLE);

    if (i_this->_38) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRC_ALPHA, GX_BL_INV_SRC_ALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

    GXColor clearColor = {0, 0, 0, 0};
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, clearColor);
    GXSetFogRangeAdj(GX_DISABLE, 0, nullptr);
    GXSetCullMode(GX_CULL_NONE);
    GXSetDither(GX_ENABLE);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetNumIndStages(0);

    GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);
    GXPosition1x8(0);
    GXPosition1x8(1);
    GXPosition1x8(2);
    GXEnd();

    resetVcdVatCache();
}

//-------------------------------------------------------
//                         Line
//-------------------------------------------------------

static J3DPacket__vtable_t mDoExt_linePacket__vtable {
    (void*)nullptr,  // RTTI
    (void*)nullptr,  // pad
    (void*)&J3DPacket__isSame,
    (void*)&J3DPacket__entry,
    (void*)&mDoExt_linePacket__draw,
    (void*)&mDoExt_linePacket__dtor,
};

KEEP_FUNC void dDbVw_drawLineXlu(cXyz& pointA, cXyz& pointB, const GXColor& color, u8 param_3, u8 width) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        mDoExt_linePacket* line = new mDoExt_linePacket(pointA, pointB, color, param_3, width);
        line->base.vtable = &mDoExt_linePacket__vtable;

        dDbVw_setDrawPacketList(&line->base, 1);
    }
}

void mDoExt_linePacket__dtor(mDoExt_linePacket* i_this) {
    i_this->~mDoExt_linePacket();
    delete i_this;
}

void mDoExt_linePacket__draw(mDoExt_linePacket* i_this) {
    J3DSys__reinitGX(&j3dSys);

    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXLoadPosMtxImm(j3dSys.getViewMtx(), 0);
    GXSetCurrentMtx(0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetZCompLoc(GX_ENABLE);

    if (i_this->_2C) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRC_ALPHA, GX_BL_INV_SRC_ALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);

    GXColor clearColor = {0, 0, 0, 0};
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, clearColor);
    GXSetFogRangeAdj(GX_DISABLE, 0, nullptr);
    GXSetCullMode(GX_CULL_NONE);
    GXSetDither(GX_ENABLE);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetNumIndStages(0);
    GXSetLineWidth(i_this->mWidth, GX_TO_ZERO);

    GXBegin(GX_LINES, GX_VTXFMT0, 2);
    GXPosition3f32(i_this->mPointA.x, i_this->mPointA.y, i_this->mPointA.z);
    GXPosition3f32(i_this->mPointB.x, i_this->mPointB.y, i_this->mPointB.z);
    GXEnd();

    resetVcdVatCache();
}

//-------------------------------------------------------
//                   Cylinder Matrix
//-------------------------------------------------------

static J3DPacket__vtable_t mDoExt_cylinderMPacket__vtable {
    (void*)nullptr,  // RTTI
    (void*)nullptr,  // pad
    (void*)&J3DPacket__isSame,
    (void*)&J3DPacket__entry,
    (void*)&mDoExt_cylinderMPacket__draw,
    (void*)&mDoExt_cylinderMPacket__dtor,
};

KEEP_FUNC void dDbVw_drawCylinderMXlu(Mtx m, const GXColor& color, u8 param_2) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        mDoExt_cylinderMPacket* cylm = new mDoExt_cylinderMPacket(m, color, param_2);
        cylm->base.vtable = &mDoExt_cylinderMPacket__vtable;

        dDbVw_setDrawPacketList(&cylm->base, 1);
    }
}

void mDoExt_cylinderMPacket__dtor(mDoExt_cylinderMPacket* i_this) {
    i_this->~mDoExt_cylinderMPacket();
    delete i_this;
}

void mDoExt_cylinderMPacket__draw(mDoExt_cylinderMPacket* i_this) {
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_ENABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->mColor);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

    if (i_this->_44) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRC_ALPHA, GX_BL_INV_SRC_ALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_BACK);
    GXSetClipMode(GX_CLIP_ENABLE);

    PSMTXConcat(j3dSys.getViewMtx(), i_this->mMatrix, i_this->mMatrix);

    GXLoadPosMtxImm(i_this->mMatrix, 0);
    mDoMtx_inverseTranspose(i_this->mMatrix, i_this->mMatrix);

    GXLoadNrmMtxImm(i_this->mMatrix, 0);
    GXSetCurrentMtx(0);

    GXDrawCylinder(8);
}

//-------------------------------------------------------
//                        Circle
//-------------------------------------------------------

static J3DPacket__vtable_t mDoExt_circlePacket__vtable {
    (void*)nullptr,  // RTTI
    (void*)nullptr,  // pad
    (void*)&J3DPacket__isSame,
    (void*)&J3DPacket__entry,
    (void*)&mDoExt_circlePacket__draw,
    (void*)&mDoExt_circlePacket__dtor,
};

KEEP_FUNC void dDbVw_drawCircleXlu(cXyz& i_position, f32 i_radius, const GXColor& i_color, u8 param_3, u8 i_lineWidth) {
    if (l_drawPacketListNum < DRAW_PACKET_MAX) {
        mDoExt_circlePacket* circle = new mDoExt_circlePacket(i_position, i_radius, i_color, param_3, i_lineWidth);
        circle->base.vtable = &mDoExt_circlePacket__vtable;

        dDbVw_setDrawPacketList(&circle->base, 1);
    }
}

void mDoExt_circlePacket__dtor(mDoExt_circlePacket* i_this) {
    i_this->~mDoExt_circlePacket();
    delete i_this;
}

void mDoExt_circlePacket__draw(mDoExt_circlePacket* i_this) {
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevColor(GX_TEVREG0, i_this->m_color);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);

    if (i_this->_24) {
        GXSetZMode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    } else {
        GXSetZMode(GX_DISABLE, GX_LEQUAL, GX_DISABLE);
    }

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRC_ALPHA, GX_BL_INV_SRC_ALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
    GXSetCullMode(GX_CULL_NONE);
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetLineWidth(i_this->m_lineWidth, GX_TO_ZERO);
    GXLoadPosMtxImm(j3dSys.getViewMtx(), 0);
    GXSetCurrentMtx(0);

    cXyz sp38;
    cXyz sp44;
    int numEdges = 36;
    sp38.y = sp44.y = i_this->m_position.y;
    
    GXBegin(GX_LINES, GX_VTXFMT0, numEdges * 2);
    for (int i = 0; i < numEdges; i++) {
        sp38.x = fcos((i * 6.2831855f) / numEdges) * i_this->m_radius;
        sp38.z = fsin((i * 6.2831855f) / numEdges) * i_this->m_radius;

        sp44.x = fcos(((i + 1) * 6.2831855f) / numEdges) * i_this->m_radius;
        sp44.z = fsin(((i + 1) * 6.2831855f) / numEdges) * i_this->m_radius;

        sp38.x += i_this->m_position.x;
        sp38.z += i_this->m_position.z;
        sp44.x += i_this->m_position.x;
        sp44.z += i_this->m_position.z;
        GXPosition3f32(sp38.x, sp38.y, sp38.z);
        GXPosition3f32(sp44.x, sp44.y, sp44.z);
    }
    GXEnd();
}

LIBTWW_DEFINE_FUNC(cM3d_UpMtx_Base__FRC3VecRC3VecPA4_f,
                  int, cM3d_UpMtx_Base, (const Vec& param_0, const Vec& param_1, Mtx m))

int cM3d_UpMtx(const Vec& param_0, Mtx m) {
    static Vec base_y = {0.0f, 1.0f, 0.0f};

    return cM3d_UpMtx_Base(base_y, param_0, m);
}

#define MAX_DRAW_DIST 2000.0f

KEEP_FUNC void dCcD_Cyl_Draw(dCcD_Cyl* i_this, const GXColor& i_color) {
    if (dComIfGp_getPlayer(0)->current.pos.abs(i_this->mCylAttr.cyl.mCenter) < MAX_DRAW_DIST) {
        dDbVw_drawCylinderXlu(i_this->mCylAttr.cyl.mCenter, i_this->mCylAttr.cyl.GetR(), i_this->mCylAttr.cyl.GetH(), i_color, 1);
    }
}

KEEP_FUNC void dCcD_Sph_Draw(dCcD_Sph* i_this, const GXColor& i_color) {
    if (dComIfGp_getPlayer(0)->current.pos.abs(i_this->mSphAttr.sph.mCenter) < MAX_DRAW_DIST) {
        dDbVw_drawSphereXlu(i_this->mSphAttr.sph.mCenter, i_this->mSphAttr.sph.GetR(), i_color, 1);
    }
}

KEEP_FUNC void dCcD_Cps_Draw(dCcD_Cps* i_this, const GXColor& i_color) {
    if (dComIfGp_getPlayer(0)->current.pos.abs(i_this->mCpsAttr.cps.mStart) < MAX_DRAW_DIST) {
        Mtx up_m;
        Mtx sp98;
        Mtx cyl_m;
        PSMTXIdentity(cyl_m);

        cXyz spD8;
        i_this->mCpsAttr.cps.CalcVec(&spD8);

        mDoMtx_trans(sp98, i_this->mCpsAttr.cps.GetStartP()->x, i_this->mCpsAttr.cps.GetStartP()->y, i_this->mCpsAttr.cps.GetStartP()->z);
        cM3d_UpMtx(spD8, up_m);
        mDoMtx_concat(sp98, up_m, cyl_m);

        mDoMtx_scale(sp98, i_this->mCpsAttr.cps.GetR(), i_this->mCpsAttr.cps.GetLen() * 0.5f, i_this->mCpsAttr.cps.GetR());
        mDoMtx_concat(cyl_m, sp98, cyl_m);
        mDoMtx_trans(sp98, 0.0f, 1.0f, 0.0f);
        mDoMtx_concat(cyl_m, sp98, cyl_m);
        mDoMtx_XrotS(sp98, 0x4000);
        mDoMtx_concat(cyl_m, sp98, cyl_m);

        dDbVw_drawCylinderMXlu(cyl_m, i_color, 1);
        dDbVw_drawSphereXlu(*i_this->mCpsAttr.cps.GetStartP(), i_this->mCpsAttr.cps.GetR(), i_color, 1);
        dDbVw_drawSphereXlu(*i_this->mCpsAttr.cps.GetEndP(), i_this->mCpsAttr.cps.GetR(), i_color, 1);
    }
}

KEEP_FUNC void dCcD_DrawCc(cCcD_Obj* i_obj, const GXColor& i_color) {
    // stupid hack, but need to differentiate between collider types
    // and this is the only decent way i know how
    if (i_obj->vtable == &dCcD_Sph_vtable) {
        dCcD_Sph_Draw((dCcD_Sph*)i_obj, i_color);
    } else if (i_obj->vtable == &dCcD_Cyl_vtable) {
        dCcD_Cyl_Draw((dCcD_Cyl*)i_obj, i_color);
    } else if (i_obj->vtable == &dCcD_Cps_vtable) {
        dCcD_Cps_Draw((dCcD_Cps*)i_obj, i_color);
    }
}

int dCcS_Data::at_obj_count = 0;
int dCcS_Data::tg_obj_count = 0;
int dCcS_Data::co_obj_count = 0;

KEEP_FUNC void GZ_drawCc(dCcS* i_this) {
    fopAc_ac_c* player = dComIfGp_getPlayer(0);
    if (player != NULL) {
        if (g_collisionFlags[VIEW_AT_CC].active) {
            for (int i = 0; i < dCcS_Data::at_obj_count; i++) {
                cCcD_Obj* obj = i_this->mpObjAt[i];
                if (obj != NULL) {
                    GXColor at_color = {0xFF, 0x00, 0x00, g_geometryOpacity};
                    dCcD_DrawCc(obj, at_color);
                }
            }
        }

        if (g_collisionFlags[VIEW_TG_CC].active) {
            for (int i = 0; i < dCcS_Data::tg_obj_count; i++) {
                cCcD_Obj* obj = i_this->mpObjTg[i];
                if (obj != NULL) {
                    GXColor tg_color = {0x3A, 0x82, 0xF0, g_geometryOpacity};
                    dCcD_DrawCc(obj, tg_color);
                }
            } 
        }

        if (g_collisionFlags[VIEW_CO_CC].active) {
            for (int i = 0; i < dCcS_Data::co_obj_count; i++) {
                cCcD_Obj* obj = i_this->mpObjCo[i];
                if (obj != NULL) {
                    GXColor co_color = {0xFF, 0xFF, 0xFF, g_geometryOpacity};
                    dCcD_DrawCc(obj, co_color);
                }
            }
        }
    }
}