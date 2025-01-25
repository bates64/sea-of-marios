#include "common.h"
#include "model.h"
#include "ship.h"
#include "dx/debug_menu.h"

struct ShipModels {
    s32 sail;
    s32 mast;
    s32 hull;

    void clone_from(const ShipModels& other) {
        clone_model(other.sail, sail);
        get_model_from_list_index(get_model_list_index_from_tree_index(sail))->flags &= ~MODEL_FLAG_HIDDEN;
        clone_model(other.mast, mast);
        get_model_from_list_index(get_model_list_index_from_tree_index(mast))->flags &= ~MODEL_FLAG_HIDDEN;
        clone_model(other.hull, hull);
        get_model_from_list_index(get_model_list_index_from_tree_index(hull))->flags &= ~MODEL_FLAG_HIDDEN;
    }
} originalModels;

API_CALLABLE(RegisterShip) {
    Bytecode* args = script->ptrReadPos;
    originalModels.sail = evt_get_variable(script, *args++);
    originalModels.mast = evt_get_variable(script, *args++);
    originalModels.hull = evt_get_variable(script, *args++);

    s32 ids[] = { originalModels.sail, originalModels.mast, originalModels.hull };
    for (auto id : ids) {
        get_model_from_list_index(get_model_list_index_from_tree_index(id))->flags |= MODEL_FLAG_HIDDEN;
    }

    return ApiStatus_DONE2;
}

struct Ship {
    bool enabled;
    ShipModels models;
    s16 areaID;
    s16 mapID;
    Vec3f pos;
    Vec3f rot;

    bool is_on_this_map() const {
        return enabled && areaID == gGameStatusPtr->areaID && mapID == gGameStatusPtr->mapID;
    }

    ShipModels* upsert_models() {
        if (!is_on_this_map()) return nullptr;

        s32 treeIndex = get_model_list_index_from_tree_index(models.sail);
        if (treeIndex == 0) {
            // Model not found (probably because we just entered this map), create them
            models.clone_from(originalModels);
        }
        return &models;
    }

    Model* sail() {
        auto* models = upsert_models();
        return models ? get_model_from_list_index(get_model_list_index_from_tree_index(models->sail)) : nullptr;
    }

    Model* mast() {
        auto* models = upsert_models();
        return models ? get_model_from_list_index(get_model_list_index_from_tree_index(models->mast)) : nullptr;
    }

    Model* hull() {
        auto* models = upsert_models();
        return models ? get_model_from_list_index(get_model_list_index_from_tree_index(models->hull)) : nullptr;
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
        ship.models.sail = CLONED_MODEL(i * 3 + 0);
        ship.models.mast = CLONED_MODEL(i * 3 + 1);
        ship.models.hull = CLONED_MODEL(i * 3 + 2);
        ship.areaID = gGameStatus.areaID;
        ship.mapID = gGameStatus.mapID;
        ship.pos = gPlayerStatus.pos;
        ship.rot = { 0.0f, 0.0f, 0.0f };

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
        if (!ship.is_on_this_map()) continue;

        Model* models[] = { ship.sail(), ship.mast(), ship.hull() };
        for (auto* model : models) {
            Matrix4f trans, rot;
            guTranslateF(trans, ship.pos.x, ship.pos.y, ship.pos.z);
            guRotateRPYF(rot, ship.rot.x, ship.rot.y, ship.rot.z);
            guMtxCatF(rot, trans, model->userTransformMtx);
            model->flags |= (MODEL_FLAG_HAS_TRANSFORM | MODEL_FLAG_MATRIX_DIRTY);
        }

        ship.rot.x += 1.0f; // rotate 1 degrees per frame
    }
}
