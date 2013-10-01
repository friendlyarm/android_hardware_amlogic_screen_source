/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "screen_source"
#include <hardware/hardware.h>
#include <hardware/aml_screen.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <linux/videodev.h>
#include <sys/time.h>


#include <errno.h>
#include <cutils/log.h>
#include <cutils/atomic.h>

#include "v4l2_vdin.h"

#ifndef LOGD
#define LOGD ALOGD
#endif
#ifndef LOGV
#define LOGV ALOGV
#endif
#ifndef LOGE
#define LOGE ALOGE
#endif
#ifndef LOGI
#define LOGI ALOGI
#endif
/*****************************************************************************/

static int aml_screen_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

static struct hw_module_methods_t aml_screen_module_methods = {
    open: aml_screen_device_open
};

aml_screen_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: AML_SCREEN_HARDWARE_MODULE_ID,
        name: "aml screen source module",
        author: "Amlogic",
        methods: &aml_screen_module_methods,
        dso : NULL,
        reserved : {0},
    }
};

/*****************************************************************************/

static int aml_screen_device_close(struct hw_device_t *dev)
{
	android::vdin_screen_source* source = NULL;
    aml_screen_device_t* ctx = (aml_screen_device_t*)dev;
    if (ctx) {
    	
    	if (ctx->priv)
    	{
    	    source = (android::vdin_screen_source*)ctx->priv;
    	    delete source;
    	    source = NULL;
    	}
    	
        free(ctx);
    }
    return 0;
}

int screen_source_start(struct aml_screen_device* dev)
{
    android::vdin_screen_source* source = (android::vdin_screen_source*)dev->priv;
	LOGV("screen_source_start");
    return source->start();
}
int screen_source_stop(struct aml_screen_device* dev)
{
    android::vdin_screen_source* source = (android::vdin_screen_source*)dev->priv;
    return source->stop();
}
int screen_source_get_format(struct aml_screen_device* dev)
{
    android::vdin_screen_source* source = (android::vdin_screen_source*)dev->priv;
    return source->get_format();
}
int screen_source_set_format(struct aml_screen_device* dev, int width, int height, int pix_format)
{
    android::vdin_screen_source* source = (android::vdin_screen_source*)dev->priv;
    
    if ((width > 0) && (height > 0) && ((pix_format == V4L2_PIX_FMT_NV21) || (pix_format == V4L2_PIX_FMT_YUV420)))
    {
        return source->set_format(width, height, pix_format);
    }
    else
    {
        return source->set_format();
    }
}
char* screen_source_aquire_buffer(struct aml_screen_device* dev)
{
    android::vdin_screen_source* source = (android::vdin_screen_source*)dev->priv;
	char * ptr_return;

	ptr_return = source->aquire_buffer();

    return ptr_return;
}
int screen_source_release_buffer(struct aml_screen_device* dev, char* ptr)
{
    android::vdin_screen_source* source = (android::vdin_screen_source*)dev->priv;
    return source->release_buffer(ptr);
}
/*****************************************************************************/

static int aml_screen_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
    int status = -EINVAL;
    android::vdin_screen_source* source = NULL;
	
    LOGV("aml_screen_device_open");
	
    if (!strcmp(name, AML_SCREEN_SOURCE)) {
        aml_screen_device_t *dev = (aml_screen_device_t*)malloc(sizeof(aml_screen_device_t));
        
        if (!dev)
        {
        	LOGE("no memory for the screen source device");
            return -ENOMEM;
        }
        /* initialize handle here */
        memset(dev, 0, sizeof(*dev));
        
        source = new android::vdin_screen_source;
        if (!source)
        {
            LOGE("no memory for class of vdin_screen_source");
            free (dev);
            return -ENOMEM;
        }
        dev->priv = (void*)source;
        
        /* initialize the procs */
        dev->common.tag         = HARDWARE_DEVICE_TAG;
        dev->common.version     = 0;
        dev->common.module      = const_cast<hw_module_t*>(module);
        dev->common.close       = aml_screen_device_close;
        
        dev->ops.start          = screen_source_start;
        dev->ops.stop           = screen_source_stop;
        dev->ops.get_format     = screen_source_get_format;
        dev->ops.set_format     = screen_source_set_format;
        dev->ops.aquire_buffer  = screen_source_aquire_buffer;
        dev->ops.release_buffer = screen_source_release_buffer;
        
        *device = &dev->common;
        
        status = 0;
    }
    return status;
}