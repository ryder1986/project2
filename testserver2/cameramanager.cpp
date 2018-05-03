#include "cameramanager.h"

CameraManager::CameraManager(JsonValue cfg,QObject *parent) : QObject(parent)
{
    cameras.clear();
    jv_2_cfg(cfg);
    start_cams();
}

