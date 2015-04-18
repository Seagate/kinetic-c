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
#include "system_test_fixture.h"
#include "kinetic_client.h"

void setUp(void)
{
    SystemTestSetup(1);
}

void tearDown(void)
{
    SystemTestShutDown();
}

void test_KineticClient_should_process_initial_unsolicited_status_response(void)
{
    int secondsWaiting = 0, maxWaiting = 2;
    while(Fixture.session->connectionID == 0) {
        LOG0("Waiting for connection ID...");
        sleep(1);
        secondsWaiting++;
        TEST_ASSERT_TRUE_MESSAGE(secondsWaiting < maxWaiting,
            "Timed out waiting for initial unsolicited status!");
    }
    TEST_ASSERT_TRUE_MESSAGE(Fixture.session->connectionID > 0, "Invalid connection ID!");
}
