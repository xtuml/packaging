// umlrtcommsport.c

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtcommsport.hh"
#include "umlrtcontroller.hh"
#include "umlrtmessage.hh"
#include "umlrtqueue.hh"

/*static*/ bool UMLRTCommsPort::purgeMatchCompare( UMLRTMessage * msg, const PurgeRecall * purge )
{
    bool match = true;
    if (purge->index != -1)
    {
        match = (purge->index == (int)msg->sapIndex0);
        if (match && (purge->id != -1))
        {
            match = (purge->id == (int)msg->signal.getId());
        }
    }
    return match;
}

/*static*/ void UMLRTCommsPort::purgeMatchNotify( UMLRTMessage * msg, const PurgeRecall * purge )
{
    umlrt::MessagePutToPool(msg);
}

int UMLRTCommsPort::purge( int index, int id ) const
{
    int count = 0;
    const PurgeRecall purge = { index, false/*front*/, id };
    if (deferQueue != NULL)
    {
        count = deferQueue->remove( (UMLRTQueue::match_compare_t)purgeMatchCompare, (UMLRTQueue::match_notify_t)purgeMatchNotify, (void *)&purge );
    }
    return count;
}


/*static*/ bool UMLRTCommsPort::recallMatchCompare( UMLRTMessage * msg, const PurgeRecall * recall )
{
    bool match = true;
    if (recall->index != -1)
    {
        match = (recall->index == (int)msg->sapIndex0);
    }
    return match;
}

/*static*/ void UMLRTCommsPort::recallMatchNotify( UMLRTMessage * msg, const PurgeRecall * recall )
{
    msg->destPort->slot->controller->recall( msg, recall->front );
}

int UMLRTCommsPort::recall( int index, bool front, bool one, int id  ) const
{
    int count = 0;
    const PurgeRecall recall = { index, front, id };

    if (deferQueue != NULL)
    {
        count = deferQueue->remove( (UMLRTQueue::match_compare_t)recallMatchCompare, (UMLRTQueue::match_notify_t)recallMatchNotify, (void *)&recall, one );
    }
    return count;
}

const UMLRTCommsPortRole * UMLRTCommsPort::role() const
{
    return border ? ((containerClass->portRolesBorder == NULL) ? NULL : &containerClass->portRolesBorder[roleIndex]) : ((containerClass->portRolesInternal == NULL) ? NULL : &containerClass->portRolesInternal[roleIndex]);
}

