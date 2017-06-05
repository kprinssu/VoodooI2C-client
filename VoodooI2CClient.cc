#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/kern_control.h>
#include <sys/kern_event.h>
#include <csignal>
#include <unistd.h>

#include <thread>

#include "updd.h"
#include "VoodooI2CClient.h"

/*
Following code is based on EchoKext (https://github.com/kentwelcome/EchoKext) by Kent Huang
*/

int fd = 0;

void disconnectWithKernel(int i)
{
    printf("[\033[0;32mo\033[0m] Disconnect to kernel...\n");
    shutdown(fd, SHUT_RDWR);
}

int main(int argc, const char * argv[]) {
    int id = 0;
    int rc = 0;
    int terminate = 0;
    struct sockaddr_ctl addr;
    ssize_t  recvSize = 0;
    
    struct gesture_socket_cmd recv_cmd;
    
    fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
    if (fd == -1) {
        printf("[\033[0;31mx\033[0m] Fail to create socket\n");
        return -1;
    }
    
    struct ctl_info ctlInfo;
    bzero(&ctlInfo, sizeof(struct ctl_info));
    strcpy(ctlInfo.ctl_name, GESTURE_CTL_NAME);
    
    for(;;) {
        
        if (ioctl(fd, CTLIOCGINFO, &ctlInfo) == -1) {
            usleep(100);
        } else break;
    }
    // Init
    bzero(&addr, sizeof(addr));
    addr.sc_len     = sizeof(addr);
    addr.sc_family  = AF_SYSTEM;
    addr.ss_sysaddr = AF_SYS_CONTROL;
    addr.sc_id      = ctlInfo.ctl_id;
    addr.sc_unit    = 0;
    
    updd_start();
    printf("[\033[0;32mo\033[0m] Created UPDD connection...\n");

    printf("[\033[0;32mo\033[0m] Create socket... fd=%d\n",fd);
    
    rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc) {
        printf("[\033[0;31mx\033[0m] Fail to connect to kernel... rc=%d\n",rc);
        return -1;
    }
    
    printf("[\033[0;32mo\033[0m] Connect to kernel... rc=%d\n",rc);
    
    rc = setsockopt(fd, SYSPROTO_CONTROL, 123, NULL, 0);
    if (rc) {
        printf("[\033[0;31mx\033[0m] Fail to send command to kernel... rc=%d\n",rc);
    }
    printf("[\033[0;32mo\033[0m] Send command to kernel... rc=%d\n",rc);
    
    signal(SIGTERM, disconnectWithKernel);
    signal(SIGINT, disconnectWithKernel);
    
    for(;;) {
        if(terminate == 1) break;

        recvSize = recv(fd, &recv_cmd, sizeof(struct gesture_socket_cmd), 0);

        if(recvSize == 0) {
            terminate = 1;
            continue;
        }

        switch(recv_cmd.type) {
            case GESTURE_DATA: {
                struct csgesture_softc *sc = new csgesture_softc();
                memcpy(sc, &recv_cmd.gesture, sizeof(struct csgesture_softc));

                std::thread inject_thread(inject_touch, sc);
                inject_thread.detach();
                break;
            }
            case GESTURE_QUIT:
                terminate = 1;
                break;
            default:
                break;
        }
    }

    updd_stop();
    printf("[\033[0;32mo\033[0m] Stopped UPDD connection...\n");
    
    shutdown(fd, SHUT_RDWR);
    printf("[\033[0;32mo\033[0m] Disconnect to kernel...\n");    
    
    return 0;
}
