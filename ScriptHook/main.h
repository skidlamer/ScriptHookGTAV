#pragma once

void scriptRegister(HMODULE module, void(*LP_SCRIPT_MAIN)());
void scriptRegisterAdditionalThread(HMODULE module, void(*LP_SCRIPT_MAIN)());
void scriptUnregister(HMODULE module);
