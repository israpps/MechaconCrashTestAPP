#include <debug.h>
#include <stdlib.h>
#include <fcntl.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <kernel.h>
#include <libcdvd.h>
#include <loadfile.h>
#include <ps2sdkapi.h>
#include <sbv_patches.h>
#include <sifrpc.h>
#include <string.h>
//#include <fileio.h>
#include <stdio.h>

char ROMVER[14];
char getMechaVersion(u8 *data);
const char* GetDspVersionAlias(u8 version);
int sceGetDspVersion(u8 *buffer, u32 *stat);

#define KAMIKAZE_DSP 3 // potential danger. CXD3098Q is the DSP revision that crashes
#define UNKNWON_DSP 4
static const char *DSP_REV[] = {
    "CXD1869Q", // 0
    "CXD1869AQ", // 1
    "CXD1869BQ/CXD1886Q-1/CXD1886", // 2
    "CXD3098Q/CXD1886Q-1/CXD3098AQ", // 3
    "UNKNOWN"}; // 4

u8 DSPVersion[2];
u8 MechaConVersion[3] = {0,0,0};
static unsigned long int ROMVERSION;

#define SP " "
int main()
{
    int fd;
    SifInitRpc(0);
    while (!SifIopReset("", 0)) { }; // order IOP to reboot
    //memset(&ROMVER, 0, 16); //in the middle, EE code that does not depend on IOP can be placed to use this dead time
    while (!SifIopSync()) { }; // Wait for IOP reboot to complete
    SifInitRpc(0);
    SifInitIopHeap();
    SifLoadFileInit();
    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check();
    sbv_patch_fileio();

    SifLoadStartModule("rom0:CDVDFSV", 0, NULL, NULL);
    sceCdInit(SCECdINoD);

    init_scr();
    scr_setCursor(0);
    scr_printf("\n\n");
    scr_printf("====================== PS2 MECHACON CRASH TEST PROGRAM "
               "======================\n");
    scr_printf("\n\n");
    scr_printf("By: El_Isra. based on mechapwn and PS2IDENT\n");
    scr_printf("Built %s\n", __DATE__);
    u32 stat;
    if (sceGetDspVersion(DSPVersion, &stat) == 0 || (stat & 0x80) != 0) {
        scr_setfontcolor(0x00ffff);
        scr_printf(" >>>> Failed to read DSP version. Stat: %x\n", stat);
        scr_setfontcolor(0xffffff);
    } else {
        scr_printf(" >>>> DSP: v%d.%02d (%s)\n", DSPVersion[0], DSPVersion[1],
                   GetDspVersionAlias(DSPVersion[1]));
    }

    if (!getMechaVersion(MechaConVersion)) {
        scr_setfontcolor(0x00ffff);
        scr_printf(" >>>> Failed to read MECHACON version.\n");
        scr_setfontcolor(0xffffff);
    } else {
        scr_printf(" >>>> MECHACON version: %d.%02d\n", MechaConVersion[1], MechaConVersion[2]);
    }
    
    if ((fd = open("rom0:ROMVER", O_RDONLY)) >= 0) {
        read(fd, ROMVER, 14);
        close(fd);
        scr_printf(" >>>> ROMVER: %s\n", ROMVER);
    } else {scr_printf(" >>>> FAILED TO GET ROMVER\n");}
    char ROMVNUM[4 + 1];
    strncpy(ROMVNUM, ROMVER, 4);
    ROMVNUM[4] = '\0';
    ROMVERSION  = strtoul(ROMVNUM, NULL, 10);
    scr_printf(" >>>> BOOT ROM VERSION: %ld\n", ROMVERSION);
    int is_PSX = 0;
    int is_fat = (ROMVERSION <= 190);
    int is_safe = 0;
    if (is_fat) {
        is_safe = !(ROMVERSION == 160 || ROMVERSION == 170 || ROMVERSION == 190); // assume console is unsafe if it is 39k or 50k
        if (!is_safe) {
            if (DSPVersion[1] == 0 || DSPVersion[1] == 1 || DSPVersion[1] == 2) is_safe = 1; //explicitly check safe DSP revisions that we can precisely ident
        }
    } else {
        is_safe = (MechaConVersion[1] == 6 && MechaConVersion[2] >= 04);
    }
    if ((fd = open("rom0:PSXVER", O_RDONLY)) >= 0) {
        close(fd);
        is_PSX = 1; is_safe = 1;
    }

    scr_printf(" >>>> DIAGNOSIS:\n");
    scr_printf("\t> CONSOLE: %s\n", (is_PSX) ? "PSX" : ((is_fat) ? "FAT" : "SLIM"));
    if (is_safe) {
        scr_setfontcolor(0x00ff00);
            scr_printf("\t> THIS CONSOLE IS SAFE FROM MECHACON CRASH\n");
        if ((!is_fat) && (!is_PSX)) scr_printf("\t\tIT HAS FACTORY PROTECTION\n");
    } else {
        scr_setfontcolor(0x0000ff);
        scr_printf("\t> THIS CONSOLE CAN SUFFER MECHACON CRASH\n");
        scr_printf("\t> IF YOU WANT TO USE BURNT DISCS INSTALL PICFix TO PROTECT THE CONSOLE\n");
    }

    scr_setfontcolor(0xffffff);
    scr_printf("\n\nProgram finished. returning to main menu in 60 seconds\n");
    sleep(60);
    return 0;
}

/* Thanks to krat0s researches this seems to return DSP version  */
int sceGetDspVersion(u8 *buffer, u32 *stat)
{
    int result;
    unsigned char subcommand, out_buffer[16];

    subcommand = 1;
    if ((result = sceCdApplySCmd(0x03, &subcommand, sizeof(subcommand),
                                 out_buffer)) != 0) {
        *stat = out_buffer[0];
    }

    /* 2 bytes: minor and major version */
    memcpy(buffer, out_buffer, sizeof(out_buffer));

    return result;
}

const char* GetDspVersionAlias(u8 version)
{
    if (version > 4)
        version = 4;
    return DSP_REV[version];
}

char getMechaVersion(u8 *data)
{
	u8 input[1];
	u8 output[16];
	input[0] = 0x00;
	if (sceCdApplySCmd(0x03, input, sizeof(input), output) != 1)
	{
		return 0;
	}

	memcpy(data, &output[0], 3);
	
	return 1;
}

PS2_DISABLE_AUTOSTART_PTHREAD();