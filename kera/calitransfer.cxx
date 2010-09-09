/*
 *  calitransfer.xx
 *
 *  Created by Daniel Wegner on 9/1/10.
 *  Copyright 2010 Karlsruhe Institute of Technology. All rights reserved.
 *
 */

#include "KFileTransfer.h"

int main() {

    KFileTransfer trans("gadamc","ccali.in2p3.fr","/sps/edelweis/EdwRootAna/ID_Run12/NeutronS2/rootevts/","/var/tmp/.");
    trans.Transfer("je25b003_004.root");
}

