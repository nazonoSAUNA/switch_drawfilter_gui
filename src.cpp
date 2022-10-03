#include <windows.h>
#include <filter.h>

FILTER_DLL filter_dll = {
    FILTER_FLAG_ALWAYS_ACTIVE,// | FILTER_FLAG_NO_CONFIG,
    NULL,NULL,
    const_cast<char*>("描画GUI非表示"),
    NULL,NULL,NULL,
    NULL,NULL,
    NULL,NULL,NULL,
    NULL,
    func_init,
    NULL,
    NULL,
    func_WndProc
};
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable() {
    return &filter_dll;
}

int exedit_dll_hinst;

BOOL exedit_Replace16(int exedit_offset, short new_value) {
    DWORD oldProtect;
    short* address = (short*)(exedit_offset + exedit_dll_hinst);
    if (!VirtualProtect(address, 2, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return FALSE;
    }
    *address = (int)new_value;
    return VirtualProtect(address, 2, oldProtect, &oldProtect);
}

int init_exeditfp(FILTER* fp) {
    SYS_INFO si;
    fp->exfunc->get_sys_info(NULL, &si);

    for (int i = 0; i < si.filter_n; i++) {
        FILTER* efp = (FILTER*)fp->exfunc->get_filterp(i);
        if (efp->information != NULL) {
            if (!lstrcmpA(efp->information, "拡張編集(exedit) version 0.92 by ＫＥＮくん")) {
                return (int)efp->dll_hinst;
            }
        }
    }
    return 0;
}

BOOL func_init(FILTER* fp) {
    exedit_dll_hinst = init_exeditfp(fp);
    if (exedit_dll_hinst == 0) {
        MessageBoxA(fp->hwnd, "拡張編集0.92が見つかりませんでした", fp->name, MB_OK);
        return FALSE;
    }
    return TRUE;
}
BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, void* editp, FILTER* fp) {
    switch (message) {
    case WM_FILTER_CHANGE_WINDOW:
        if (fp->exfunc->is_filter_window_disp(fp)) {
            SetWindowPos(fp->hwnd, 0, 0, 0, 0, 0, (SWP_NOSIZE | SWP_HIDEWINDOW));
            exedit_Replace16(0x19c65, 0xe990); // JMP
        } else {
            exedit_Replace16(0x19c65, 0x840f); // JZ
        }
        return TRUE;
    }
    return FALSE;
}
