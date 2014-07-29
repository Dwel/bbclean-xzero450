#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "AgentMaster.h"
#include "Definitions.h"
#include "MenuMaster.h"
#include "BBApi.h" // only for cfg, @TODO: remove if not needed
#include "ConfigMaster.h" // cfg
#include "AgentType_Mixer_Vista.h"


int agenttype_mixer_startup_vista ()
{
	CoInitialize(NULL);
	//Register this type with the ControlMaster
	agent_registertype(
		"Mixer",                            //Friendly name of agent type
		"MixerScale",                       //Name of agent type
		CONTROL_FORMAT_SCALE|CONTROL_FORMAT_TEXT,               //Control format
		true,
		&agenttype_mixer_create_vista,
		&agenttype_mixer_destroy_vista,
		&agenttype_mixer_message_vista,
		&agenttype_mixer_notify_vista,
		&agenttype_mixer_getdata_vista,
		&agenttype_mixerscale_menu_set_vista,
		&agenttype_mixer_menu_context_vista,
		&agenttype_mixer_notifytype_vista
		);

	//Register this type with the ControlMaster
	agent_registertype(
		"Mixer",                            //Friendly name of agent type
		"MixerBool",                        //Name of agent type
		CONTROL_FORMAT_BOOL,                //Control format
		true,
		&agenttype_mixer_create_vista,
		&agenttype_mixer_destroy_vista,
		&agenttype_mixer_message_vista,
		&agenttype_mixer_notify_vista,
		&agenttype_mixer_getdata_vista,
		&agenttype_mixerbool_menu_set_vista,
		&agenttype_mixer_menu_context_vista,
		&agenttype_mixer_notifytype_vista
		);

	return 0;
}

int agenttype_mixer_shutdown_vista ()
{
	CoUninitialize();
	return 0;
}

struct AgentType_Mixer_Vista
{
	AgentType_Mixer_Vista (long (& values)[3])
		: m_device(values[0])
		, m_line(values[1])
		, m_control(values[2])
		, m_endpoint(0)
		, m_value_double(0.0)
		, m_value_bool(false)
	{ }

	bool Init ();
	void Destroy ();

	float GetVolume () const;
	void SetVolume (float v) const;

	bool GetMute () const;
	void SetMute (bool m) const;

	long m_device;
	long m_line;
	long m_control;
	
	IAudioEndpointVolume * m_endpoint;
	double m_value_double;
	bool m_value_bool;
	char m_value_text[32];
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixer_create
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int agenttype_mixer_create_vista (agent * a, char * parameterstring)
{
	bool errorflag = false; //If there's an error

	//Check for error conditions
	if (strlen(parameterstring) >= 30) return false;
	//Break up the parts
	char mixertoken1[30], mixertoken2[30], mixertoken3[30], mixertoken4[30];
	char * mixer_tokenptrs[4] = {mixertoken1, mixertoken2, mixertoken3, mixertoken4};
	int tokensfound = BBTokenize(parameterstring, mixer_tokenptrs, 4, NULL);
	//Three tokens exactly required
	if (tokensfound != 3) return 1;

	long values[3];
	//Make sure they are all valid integers
	for (int i = 0; i < 3; i++)
		if (!config_set_long(mixer_tokenptrs[i], &values[i]))
			return 1;

	AgentType_Mixer_Vista * details = new AgentType_Mixer_Vista(values);
	if (details->Init())
	{
		a->agentdetails = static_cast<void *>(details);
	}
	else
	{
		details->Destroy();
		delete details;
		a->agentdetails = NULL;

		agent_destroy(&a);
		return 1;   
	}
	return 0;
}

int agenttype_mixer_destroy_vista (agent * a)
{
	if (a->agentdetails)
	{
		AgentType_Mixer_Vista * details = static_cast<AgentType_Mixer_Vista *>(a->agentdetails);
		details->Destroy();
		delete details;
		a->agentdetails = NULL;
	}
	return 0;
}

int agenttype_mixer_message_vista (agent *a, int tokencount, char *tokens[])
{
	return 1;
}

void agenttype_mixer_notify_vista (agent *a, int notifytype, void *messagedata)
{
	if (a->agentdetails)
	{
		AgentType_Mixer_Vista * details = static_cast<AgentType_Mixer_Vista *>(a->agentdetails);

		switch (notifytype)
		{
			case NOTIFY_CHANGE:
			{
				double * value_double = static_cast<double *>(messagedata);
				if (value_double)
				{
					float volume = static_cast<float>(*value_double);
					details->SetVolume(volume);
				}
				break;
			}
			case NOTIFY_SAVE_AGENT:
			{
				char temp[30];
				sprintf(temp, "%d %d %d", (int)details->m_device, (int)details->m_line, (int)details->m_control);
				config_write(config_get_control_setagent_c(a->controlptr, a->agentaction, a->agenttypeptr->agenttypename, temp));
				break;
			}
		}
	}
}

void * agenttype_mixer_getdata_vista (agent *a, int datatype)
{
	if (a->agentdetails)
	{
		AgentType_Mixer_Vista * details = static_cast<AgentType_Mixer_Vista *>(a->agentdetails);

		details->m_value_double = details->GetVolume();
		details->m_value_bool = details->GetMute();

		switch (datatype)
		{
			case DATAFETCH_VALUE_TEXT:
			{
				int const intvalue = static_cast<int>(100.0 * details->m_value_double);
				sprintf(details->m_value_text, "%d%%", intvalue);
				return details->m_value_text;
			}
			case DATAFETCH_VALUE_SCALE:
				return &(details->m_value_double);
			case DATAFETCH_VALUE_BOOL:
				return &(details->m_value_bool);
		}
	}
	return NULL;
}

void agenttype_mixerscale_menu_set_vista (Menu *m, control *c, agent *a,  char *action, int controlformat)
{
}

void agenttype_mixerbool_menu_set_vista (Menu *m, control *c, agent *a,  char *action, int controlformat)
{
}

void agenttype_mixer_menu_context_vista (Menu *m, agent *a)
{
	make_menuitem_nop(m, "No options available.");
}

void agenttype_mixer_notifytype_vista (int notifytype, void *messagedata)
{
}

/////////////////////////////////////////////

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

bool AgentType_Mixer_Vista::Init ()
{
	IMMDeviceEnumerator * deviceEnumerator = NULL;
	if (S_OK == CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator))
	{
		IMMDevice * defaultDevice = NULL;

		if (S_OK == deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice))
		{
			deviceEnumerator->Release();
			deviceEnumerator = NULL;

			IAudioEndpointVolume * endpointVolume = NULL;
			if (S_OK == defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume))
			{
				defaultDevice->Release();
				defaultDevice = NULL;
				m_endpoint = endpointVolume;
				return true;
			}
		}
	}
	return false;
}

/*
#define SAFE_RELEASE(punk)  if ((punk) != NULL)  { (punk)->Release(); (punk) = NULL; }
// Convert an UTF8 string to a wide Unicode String
int utf8_encode (LPWSTR * wstr, char * buff, size_t buff_sz)
{
	const size_t w_size = wcslen(wstr);
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, w_size, NULL, 0, NULL, NULL);
	if (size_needed > buff_sz)
		size_needed = buff_sz;
    WideCharToMultiByte(CP_UTF8, 0, wstr, w_size, buff, size_needed, NULL, NULL);
    return size_needed;
}

void agenttype_mixer_menu_devices (Menu *menu, control *c, char *action, char *agentname, int format)
{
	IMMDeviceEnumerator * pEnumerator = NULL;
	IMMDeviceCollection * pCollection = NULL;
	IMMDevice * pEndpoint = NULL;

	if (S_OK == CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator))
	{
		if (S_OK == pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection))
		{
			UINT  count = 0;
			if (S_OK ==  pCollection->GetCount(&count))
			{
				if (count == 0)
				{
					printf("No endpoints found.\n");
				}

				// Each loop prints the name of an endpoint device.
				for (ULONG i = 0; i < count; i++)
				{
					if (S_OK != pCollection->Item(i, &pEndpoint))
						continue;

					LPWSTR pwszID = NULL;
					if (S_OK != pEndpoint->GetId(&pwszID))
						continue;

					IPropertyStore * pProps = NULL;
					if (S_OK == pEndpoint->OpenPropertyStore(STGM_READ, &pProps))
					{
						PROPVARIANT varName;
						PropVariantInit(&varName);
						if (S_OK == pProps->GetValue(PKEY_Device_FriendlyName, &varName))
						{
							Menu *submenu;
							submenu = make_menu(varName.pwszVal, c);
							//agenttype_mixer_menu_destlines(submenu, c, action, agentname, format, device, mixer_handle, mixer_capabilities);
							make_submenu_item(menu, varName.pwszVal, submenu);

							// Print endpoint friendly name and endpoint ID.
							//printf("Endpoint %d: \"%S\" (%S)\n", i, varName.pwszVal, pwszID);
						}
					}

					CoTaskMemFree(pwszID);
					pwszID = NULL;
					PropVariantClear(&varName);
					SAFE_RELEASE(pProps)
					SAFE_RELEASE(pEndpoint)
				}
			}
		}
	}
	SAFE_RELEASE(pEnumerator)
	SAFE_RELEASE(pCollection)
	return;
}
#undef SAFE_RELEASE
*/

float AgentType_Mixer_Vista::GetVolume () const
{
	float currentVolume = 0.0f;
	if (m_endpoint && S_OK == m_endpoint->GetMasterVolumeLevelScalar(&currentVolume))
	{
		return currentVolume;
	}
	return 0.0f;
}

void AgentType_Mixer_Vista::SetVolume (float v) const
{
	if (m_endpoint)
		m_endpoint->SetMasterVolumeLevelScalar(v, NULL);
}

void AgentType_Mixer_Vista::Destroy ()
{
	if (m_endpoint)
	{
		m_endpoint->Release();
		m_endpoint = NULL;
	}
}


bool AgentType_Mixer_Vista::GetMute () const
{
	BOOL mute = 0;
	if (m_endpoint && S_OK == m_endpoint->GetMute(&mute))
	{
		return mute;
	}
	return false;
}

void AgentType_Mixer_Vista::SetMute (bool m) const
{

}



