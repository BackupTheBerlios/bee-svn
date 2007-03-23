#include <stdio.h>

delete_msgs(  )
{
        imap_open( host, port );
        imap_login( "user", "user" );
        imap_select( "inbox" );
        imap_store( "1:* +flags (\\delete)" );
        imap_expunge(  );
}

delete_folders(  )
{
        imap_open( host, port );
        imap_login( "user", "user" );
        struct folder fld = imap_list( "\"\"", "*" );
        for( i = fld.count; i > 0; i-- )
                imap_delete( fld.name[i] );

}

int main(  )
{
        return 0;
}
