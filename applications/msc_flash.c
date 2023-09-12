#include "fal.h"
#include "usbd_core.h"
#include "usbd_msc.h"
#include <rtdevice.h>

#define MSC_IN_EP          0x81
#define MSC_OUT_EP         0x02

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE    (9 + MSC_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define MSC_MAX_MPS 512
#else
#define MSC_MAX_MPS 64
#endif

const uint8_t msc_ram_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, MSC_MAX_MPS, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'M', 0x00,                  /* wcChar10 */
    'S', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00};

void usbd_event_handler(uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            USB_LOG_DBG("USB reset");
            break;
        case USBD_EVENT_CONNECTED:
            USB_LOG_DBG("USB connected");
            break;
        case USBD_EVENT_DISCONNECTED:
            USB_LOG_DBG("USB disconnected");
            break;
        case USBD_EVENT_RESUME:
            USB_LOG_DBG("USB resume");
            break;
        case USBD_EVENT_SUSPEND:
            USB_LOG_DBG("USB suspend");
            break;
        case USBD_EVENT_CONFIGURED:
            USB_LOG_DBG("USB configured");
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            USB_LOG_DBG("USB set remote wakeup");
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            USB_LOG_DBG("USB clr remote wakeup");
            break;

        default:
            break;
    }
}

rt_device_t flash_dev = 0;
uint32_t nsector      = 0;
uint32_t blsz         = 0;

void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
    struct rt_device_blk_geometry geometry;
    rt_memset(&geometry, 0, sizeof(geometry));
    rt_device_control(flash_dev, RT_DEVICE_CTRL_BLK_GETGEOME, &geometry);
    nsector = *block_num = (uint32_t)geometry.sector_count;
    blsz = *block_size = (uint16_t)geometry.block_size;
    USB_LOG_INFO("block_num: %d, block_size: %d", *block_num, *block_size);
}

int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    USB_LOG_DBG("read sector: %d, length: %d", sector, length);
    rt_device_read(flash_dev, sector, buffer, length / blsz);
    return 0;
}

int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length)
{
    USB_LOG_DBG("write sector: %d, length: %d", sector, length);
    rt_device_write(flash_dev, sector, buffer, length / blsz);
    return 0;
}

struct usbd_interface intf0;

void msc_ram_init(void)
{
    flash_dev = rt_device_find(FS_PARTITION_NAME);
    usbd_desc_register(msc_ram_descriptor);
    usbd_add_interface(usbd_msc_init_intf(&intf0, MSC_OUT_EP, MSC_IN_EP));
    usbd_initialize();
}
INIT_APP_EXPORT(msc_ram_init);
