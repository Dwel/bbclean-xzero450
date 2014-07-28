#include "BBApi.h"
#include <string.h>
#include <stdlib.h>
#include "PluginMaster.h"
#include "AgentMaster.h"
#include "Definitions.h"
#include "ControlMaster.h"
#include "ConfigMaster.h"
#include "MenuMaster.h"
#include "AgentType_Mixer_Vista.h"

int agenttype_mixer_startup_vista ()
{
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
	return 0;
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixer_create
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int agenttype_mixer_create_vista (agent * a, char * parameterstring)
{
	/*if (details->Init())
	{
		a->agentdetails = static_cast<void *>(details);
	}
	else
	{
		detais->Destroy();
		delete details;
		a->agentdetails = NULL;

		agent_destroy(&a);
		return 1;
	}
*/
	return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixer_destroy
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int agenttype_mixer_destroy_vista (agent * a)
{
	/*if (a->agentdetails)
	{
		AgentType_Mixer_Vista * details = static_cast<AgentType_Mixer_Vista *>(a->agentdetails);
		details->Destroy();
		delete details;
		a->agentdetails = NULL;
	}*/
	return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixer_message
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int agenttype_mixer_message_vista (agent *a, int tokencount, char *tokens[])
{
	return 1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixer_notify
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void agenttype_mixer_notify_vista (agent *a, int notifytype, void *messagedata)
{
	// Get the agent details
	//agenttype_mixer_details *details;
	//details = (agenttype_mixer_details *) a->agentdetails;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixer_getdata
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void *agenttype_mixer_getdata_vista (agent *a, int datatype)
{
	//agenttype_mixer_details *details = (agenttype_mixer_details *) a->agentdetails;
	//if (!details) return NULL;

	return NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixerscale_menu_set
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void agenttype_mixerscale_menu_set_vista (Menu *m, control *c, agent *a,  char *action, int controlformat)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixerbool_menu_set
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void agenttype_mixerbool_menu_set_vista (Menu *m, control *c, agent *a,  char *action, int controlformat)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixer_menu_context
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void agenttype_mixer_menu_context_vista (Menu *m, agent *a)
{
	make_menuitem_nop(m, "No options available.");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//agenttype_mixer_notifytype
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void agenttype_mixer_notifytype_vista (int notifytype, void *messagedata)
{

}


