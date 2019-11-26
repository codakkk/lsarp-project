
#include <YSI_Coding\y_hooks>

#define MAX_VEHICLE_COMPONENTS 14

static VehicleComponent[MAX_VEHICLES][MAX_VEHICLE_COMPONENTS] = {0, ...};

hook OnVehicleCreate(vehicleid)
{
    for(new i = 0; i < MAX_VEHICLE_COMPONENTS; ++i)
        VehicleComponent[vehicleid][i] = 0;
    return 1;
}

task OnVehicleComponentUpdate[5000]()
{
    static componentid = 0, acComponentid = 0;
    foreach(new vehicleid : Vehicle)
    {
        for(new slot = 0; slot < MAX_VEHICLE_COMPONENTS; ++slot)
        {
            componentid = GetVehicleComponentInSlot(vehicleid, slot);
            acComponentid = VehicleComponent[vehicleid][slot];
            if(componentid != acComponentid)
            {
                RemoveVehicleComponent(vehicleid, componentid);
                AddVehicleComponent(vehicleid, acComponentid);
            }
        }
    }
}

stock bool:Vehicle_AddComponent(vehicleid, componentid)
{
    AddVehicleComponent(vehicleid, componentid);
    new type = GetVehicleComponentType(componentid);
    VehicleComponent[vehicleid][type] = componentid;
    return true;
}

