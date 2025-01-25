#include "common.h"
#include "model.h"
#include "ship.h"
#include "dx/debug_menu.h"

Model* originalModel; // All ships will clone this model
API_CALLABLE(RegisterShip) {
    s32 id = evt_get_variable(script, *script->ptrReadPos);
    originalModel = get_model_from_list_index(get_model_list_index_from_tree_index(id));
    originalModel->flags |= MODEL_FLAG_HIDDEN;
    return ApiStatus_DONE2;
}

struct Ship {
    bool enabled;
    s32 modelID;
    s16 areaID;
    s16 mapID;
    Vec3f pos;
    Vec3f rot;

    bool is_on_this_map() const {
        return enabled && areaID == gGameStatusPtr->areaID && mapID == gGameStatusPtr->mapID;
    }

    Model* model() {
        if (!is_on_this_map()) return nullptr;

        s32 treeIndex = get_model_list_index_from_tree_index(modelID);
        if (treeIndex != 0) {
            return get_model_from_list_index(treeIndex);
        }

        // Model not found (probably because we just entered this map), create one
        clone_model(originalModel->modelID, modelID);
        Model* model = get_model_from_list_index(get_model_list_index_from_tree_index(modelID));
        model->flags &= ~MODEL_FLAG_HIDDEN;
        debug_printf("created ship model %d\n", modelID);
        return model;
    }
};

Ship ships[4];

void clear_ships() {
    for (auto& ship : ships) {
        ship.enabled = false;
    }
}

void notify_ships_map_load() {
    // Map changed
}

void create_ship() {
    for (s32 i = 0; i < ARRAY_COUNT(ships); i++) {
        auto& ship = ships[i];
        if (ship.enabled) continue;

        ship.enabled = true;
        ship.modelID = CLONED_MODEL(i);
        ship.areaID = gGameStatus.areaID;
        ship.mapID = gGameStatus.mapID;
        ship.pos = gPlayerStatus.pos;
        ship.rot = { 0.0f, 0.0f, gPlayerStatus.heading };

        gPlayerStatus.pos.y += 100.0f; // TEMP so player can board the ship

        return;
    }
    PANIC_MSG("cannot create ship; all slots are full");
}

void update_ships() {
    // TEMP
    if (gPlayerStatus.pressedButtons & BUTTON_C_DOWN) {
        create_ship();
    }

    for (auto& ship : ships) {
        auto* model = ship.model();
        if (model == nullptr) continue;

        Matrix4f trans, rot;
        guTranslateF(trans, ship.pos.x, ship.pos.y, ship.pos.z);
        guRotateRPYF(rot, ship.rot.x, ship.rot.y, ship.rot.z);
        guMtxCatF(rot, trans, model->userTransformMtx);
        model->flags |= (MODEL_FLAG_HAS_TRANSFORM | MODEL_FLAG_MATRIX_DIRTY);

        ship.rot.x += 1.0f; // rotate 1 degrees per frame
    }
}
