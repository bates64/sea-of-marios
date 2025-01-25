#include "common.h"
#include "model.h"
#include "ship.h"

Model* originalModel; // All ships will clone this model
API_CALLABLE(RegisterShip) {
    s32 id = evt_get_variable(script, *script->ptrReadPos);
    originalModel = get_model_from_list_index(get_model_list_index_from_tree_index(id));
    originalModel->flags |= MODEL_FLAG_HIDDEN;
    // TEMP
    create_ship();
    return ApiStatus_DONE2;
}

struct Ship {
    bool enabled;
    Model* model;
};

Ship ships[4];

void clear_ships() {
    for (auto& ship : ships) {
        ship.enabled = false;
        ship.model = nullptr;
    }
}

void create_ship() {
    for (s32 i = 0; i < ARRAY_COUNT(ships); i++) {
        auto& ship = ships[i];
        if (ship.enabled) continue;

        ship.enabled = true;

        clone_model(originalModel->modelID, CLONED_MODEL(i));
        ship.model = get_model_from_list_index(get_model_list_index_from_tree_index(CLONED_MODEL(i)));
        ship.model->flags &= ~MODEL_FLAG_HIDDEN;

        return;
    }
    PANIC_MSG("cannot create ship; all slots are full");
}

void update_ships() {
    // TEMP
    f32 x = gPlayerStatus.pos.x;
    f32 y = gPlayerStatus.pos.y;
    f32 z = gPlayerStatus.pos.z;
    f32 yaw = gPlayerStatus.heading;

    for (auto& ship : ships) {
        if (!ship.enabled) continue;

        Matrix4f trans, rot;
        guTranslateF(trans, x, y, z);
        guRotateF(rot, 0, 1, 0, yaw);
        guMtxCatF(trans, rot, ship.model->userTransformMtx);
        ship.model->flags |= (MODEL_FLAG_HAS_TRANSFORM | MODEL_FLAG_MATRIX_DIRTY);
    }
}
