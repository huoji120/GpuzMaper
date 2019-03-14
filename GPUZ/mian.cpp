#include"Global.h"
#define GPUZ_PATH "C:\\Users\\sword\\Desktop\\gpuz.sys"
int main(int argc, char* argv[])
{
	
	OSVERSIONINFO osver;
	osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	std::string ServiceRegKey;
	auto& utils = Utils::instance();
	if (!utils.EnablePrivilege("SeLoadDriverPrivilege"))		// Enable 'SeLoadDriverPrivilege' For This Process To Load Drivers
		throw std::runtime_error("Could Not Set 'SeLoadDriverPrivilege'.");
	if (!utils.RegisterService(std::string(GPUZ_PATH), &ServiceRegKey))
	{
		printf("Could Not Register gpuz.sys.");
		getchar();
		return -1;
	}
	NTSTATUS Status = utils.UnloadDriver(ServiceRegKey);
	Status = utils.LoadDriver(ServiceRegKey);
	/* Check if It is Not A Success, But if Status == STATUS_OBJECT_NAME_COLLISION. Then It Means That It Is Already Running */
	if (!NT_SUCCESS(Status) && Status != STATUS_OBJECT_NAME_COLLISION)		// If it is already running, dont error handle.
	{
		printf("gpuz.sys Could Not Be Started.%d",GetLastError());
		getchar();
		return -1;
	}
	printf("gpuz.sys running successfully!\n");

	if (!utils.RegisterService(argv[1], &ServiceRegKey))		// Register Desired Driver
		throw std::runtime_error("Could Not Register Service.");
	GetVersionExA(&osver);			// Get BuildNumber For The OS

	sys* System = new sys(osver.dwBuildNumber);			// Initiate The System Global Var
	if (!System->LoadDriverRoutine(ServiceRegKey))			// Call The LoadDriverRoutine To Load The Desired Driver
	{
		printf("Target Driver Load Failed.%d\n", GetLastError());
		getchar();
		return -1;
	}
	System->~sys();

	getchar();
	return 0;
}