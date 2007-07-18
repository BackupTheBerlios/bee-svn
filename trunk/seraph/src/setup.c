/*
 * Copyright (C) 2006, 2007 Free Software Foundation, Inc.
 * Written by Negreanu Marius <groleo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING.  If not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * Setup SUT on remote machines
 */

/*
 * [in] os
 * [in] version
 */
setup_SUT()
{
    char src[PATH_MAX]={0}, dst[PATH_MAX]={0};

    setup_copySUTBinary("/mnt/hawaii/versions/SERVER/5.0.0.12/linux/gcc3/axigen.debug", "user@purec:/dir");
    setup_craftSUTConfig();
    setup_createSUTStorage();
    setup_backupSUTStorage();
}


/* source
 * destination
 * actor(user[username,password])
 */
setup_copySUTBinary(const char *src, const char *scpDst)
{
    struct stat st;
    int rval=0;

    rval = lstat( src, &st);
    if( rval )
    {
        dbg_error("Unable to stat [%s] : [%s]\n", src, strerror(errno));
        return false;
    }
    if( access( src,R_OK ) || !ST_ISDIR(st.st_mode))
    {
        dbg_error("Unable to read from [%s] : [%s]\n", src, strerror(errno));
        return false;
    }
    /* this can and will be replaced by libssh2 */
    rval = system("scp src dest");
    if( rval ) return false;
    return true;
}

/*
 * [in] configFileName
 * [in] (configParam, configParamValue)
 */
setup_craftSUTConfig()
{
    open(configFileName);
    for( i=0; i< plist->length(); ++i)
    {
        replace(plist->at(i))
    }
}

/* create domain
 * create user
 * delete mails from user's mbox
 */
setup_createSUTStorage()
{
    startSUT();
    createDomain(SUT_DEFDOM);
    addUser(SUT_USER);
    delUserMails(SUT_USER);
    stopSUT();
}

/** Backup SUT storage.
 * [in] backupDestDirectory
 * [in] storageSourceDirectory
 */
setup_backupSUTStorage()
{
    copy();
}

