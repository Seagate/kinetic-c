/*
* kinetic-c
* Copyright (C) 2015 Seagate Technology.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/

#ifndef _KINETIC_BUILDER_H
#define _KINETIC_BUILDER_H

#include "kinetic_types_internal.h"

/* Operations with non-standard timeouts. */
typedef enum {
    KineticOperation_TimeoutSetACL = 30,
    KineticOperation_TimeoutSetPin = 30,
    KineticOperation_TimeoutLockUnlock = 30,
    KineticOperation_TimeoutErase = 180,
} KineticOperation_NonstandardTimeout;

/*******************************************************************************
 * Standard Client Operations
*******************************************************************************/
KineticStatus KineticBuilder_BuildNoop(KineticOperation* op);
KineticStatus KineticBuilder_BuildPut(KineticOperation* const op,
    KineticEntry* const entry);
KineticStatus KineticBuilder_BuildGet(KineticOperation* const op,
    KineticEntry* const entry);
KineticStatus KineticBuilder_BuildGetNext(KineticOperation* const op,
    KineticEntry* const entry);
KineticStatus KineticBuilder_BuildGetPrevious(KineticOperation* const op,
    KineticEntry* const entry);
KineticStatus KineticBuilder_BuildFlush(KineticOperation* const op);
KineticStatus KineticBuilder_BuildDelete(KineticOperation* const op,
    KineticEntry* const entry);
KineticStatus KineticBuilder_BuildGetKeyRange(KineticOperation* const op,
    KineticKeyRange* range, ByteBufferArray* buffers);
KineticStatus KineticBuilder_BuildP2POperation(KineticOperation* const op,
    KineticP2P_Operation* const p2pOp);

/*******************************************************************************
 * Admin Client Operations
*******************************************************************************/
KineticStatus KineticBuilder_BuildGetLog(KineticOperation* const op,
    KineticProto_Command_GetLog_Type type, ByteArray name, KineticLogInfo** info);
KineticStatus KineticBuilder_BuildSetPin(KineticOperation* const op,
    ByteArray old_pin, ByteArray new_pin, bool lock);
KineticStatus KineticBuilder_BuildErase(KineticOperation* const op,
    bool secure_erase, ByteArray* pin);
KineticStatus KineticBuilder_BuildLockUnlock(KineticOperation* const op,
    bool lock, ByteArray* pin);
KineticStatus KineticBuilder_BuildSetACL(KineticOperation* const op,
    struct ACL *ACLs);
KineticStatus KineticBuilder_BuildSetClusterVersion(KineticOperation* const op,
    int64_t new_cluster_version);
KineticStatus KineticBuilder_BuildUpdateFirmware(KineticOperation* const op,
    const char* fw_path);

#endif // _KINETIC_BUILDER_H
