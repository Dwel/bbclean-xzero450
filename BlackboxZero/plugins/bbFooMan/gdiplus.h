#define GpImage		HANDLE
#define GpStatus	int
#define WINGDIPAPI	__stdcall


typedef VOID (WINAPI *DebugEventProc)(DWORD level, CHAR *message);
typedef VOID (WINAPI *GdiplusStartupInputProc)(
        DebugEventProc debugEventCallback,
        BOOL suppressBackgroundThread,
        BOOL suppressExternalCodecs);

typedef struct
{
	UINT32 GdiplusVersion;             
	DebugEventProc DebugEventCallback;
	BOOL SuppressBackgroundThread;
	BOOL SuppressExternalCodecs; 
	GdiplusStartupInputProc startproc; 
} GdiplusStartupInput;

typedef GpStatus (WINAPI *NotificationHookProc)(OUT ULONG_PTR *token);
typedef VOID (WINAPI *NotificationUnhookProc)(ULONG_PTR token);
typedef struct
{
  NotificationHookProc NotificationHook;
  NotificationUnhookProc NotificationUnhook;
} GdiplusStartupOutput;


GpStatus WINGDIPAPI GdiplusStartup(HANDLE *token, GdiplusStartupInput *input, GdiplusStartupOutput *output);
GpStatus WINGDIPAPI GdipLoadImageFromFile(WCHAR* filename, GpImage **image);
GpStatus WINGDIPAPI GdipGetImageWidth(GpImage *image, UINT *width);
GpStatus WINGDIPAPI GdipGetImageHeight(GpImage *image, UINT *height);
GpStatus WINGDIPAPI GdipDisposeImage(GpImage *image);
GpStatus WINGDIPAPI GdiplusShutdown(HANDLE *token);
