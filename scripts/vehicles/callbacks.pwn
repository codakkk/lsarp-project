
// Called when a player vehicle is saved.
forward OnPlayerVehicleSaved(vehicleid);

// Called when a player vehicle is loaded
forward OnPlayerVehicleLoaded(vehicleid);

// Called when a player vehicle is unloaded. It's called before variables are reset and before Vehicle_Save is called.
forward OnPlayerVehicleUnLoaded(vehicleid);