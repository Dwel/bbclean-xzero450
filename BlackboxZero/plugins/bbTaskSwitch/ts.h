#ifndef _TS_TS_H
#define _TS_TS_H

/**
 * NOTE: The following functions will inactivate TS temporarily.
 *
 *       TS_SetAttr, TS_ChangeEngine, TS_ChangeHotKey, TS_DebugWindow
 */

#define TS_PUBLIC(type) __declspec(dllexport) type




typedef struct _TS TS;
typedef struct _TSTaskList TSTaskList;
typedef LRESULT CALLBACK TSHandler(TS*);
typedef BOOL CALLBACK TSSieveProcedure(HWND, TSTaskList*);




#define TSM_ACTIVATE (WM_USER+0x0001)
#define TSM_INACTIVATE (WM_USER+0x0002)
#define TSM_NEXT (WM_USER+0x0003)
#define TSM_PREV (WM_USER+0x0004)
#define TSM_SWITCH (WM_USER+0x0005)
#define TSM_CANCEL (WM_USER+0x0006)


typedef enum {
	TSEngine_NONE,
	TSEngine_SIMPLE,
	TSEngine_HOOK
} TSEngine;


#define TSKey_OnStartSelecting TEXT("TS_OnStartSelecting")
#define TSKey_OnPaint TEXT("TS_OnPaint")
#define TSKey_OnCancel TEXT("TS_OnCancel")
#define TSKey_OnSwitch TEXT("TS_OnSwitch")
#define TSKey_SieveProcedure TEXT("TS_SieveProcedure")

/* read only */
#define TSKey_Window TEXT("TS_Window")
#define TSKey_Activeness TEXT("TS_Activeness")
#define TSKey_Selecting TEXT("TS_Selecting")
#define TSKey_TaskList TEXT("TS_TaskList")
#define TSKey_HotKey TEXT("TS_HotKey")
#define TSKey_Engine TEXT("TS_Engine")
#define TSKey_DebugWindow TEXT("TS_DebugWindow")




#define TS_DECLARE(TYPE, FUNC, ARGS) \
          TS_PUBLIC(TYPE) FUNC ARGS; \
          typedef TYPE FUNC##_f ARGS;


TS_DECLARE(TS*, TS_New, (HINSTANCE))
TS_DECLARE(void, TS_Del, (TS*, HINSTANCE))

TS_DECLARE(LRESULT, TS_Control, (TS*, UINT))
#define TS_Activate(ts) TS_Control((ts), TSM_ACTIVATE)
#define TS_Inactivate(ts) TS_Control((ts), TSM_INACTIVATE)

TS_DECLARE(void*, TS_GetAttr, (TS*, LPCTSTR))
TS_DECLARE(BOOL, TS_SetAttr, (TS*, LPCTSTR, void*))

TS_DECLARE(BOOL, TS_ChangeEngine, (TS*, TSEngine))
TS_DECLARE(BOOL, TS_ChangeHotKey, (TS*, UINT))
TS_DECLARE(BOOL, TS_UseDebugWindow, (TS*, BOOL))


TS_DECLARE(LRESULT CALLBACK, TS_DefaultOnStartSelecting, (TS*))
TS_DECLARE(LRESULT CALLBACK, TS_DefaultOnPaint, (TS*))
TS_DECLARE(LRESULT CALLBACK, TS_DefaultOnSwitch, (TS*))
TS_DECLARE(LRESULT CALLBACK, TS_DefaultOnCancel, (TS*))

TS_DECLARE(BOOL CALLBACK, TS_DefaultSieveProcedure, (HWND, TSTaskList*))
TS_DECLARE(BOOL CALLBACK, TS_OldSieveProcedure, (HWND, TSTaskList*))


TS_DECLARE(int, TSTaskList_GetTaskCount, (const TSTaskList*))
TS_DECLARE(int, TSTaskList_GetSelectedTaskIndex, (const TSTaskList*))
TS_DECLARE(HWND, TSTaskList_GetWindow, (const TSTaskList*, int))
TS_DECLARE(LPCTSTR, TSTaskList_GetTitle, (const TSTaskList*, int))
TS_DECLARE(ULONG_PTR*, TSTaskList_GetExtra, (const TSTaskList*, int))


#undef TS_DECLARE




#endif
