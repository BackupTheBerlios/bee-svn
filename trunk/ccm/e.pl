#! /usr/bin/perl
use Crypt::CCM;
use strict;

my $key             = pack 'H*', 'C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF';
my $nonce           = pack 'H*', '00000004030201A0A1A2A3A4A5';
my $associated_data = pack 'H*', '0001020304050607';
my $plain_text      = pack 'H*', '08090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F';
my $c = Crypt::CCM->new(
    -key    => $key,
    -cipher => 'Crypt::Rijndael',
    -tag_length => 64/8,
);
$c->set_nonce($nonce);
$c->set_aad($associated_data);
my $cipher_text = $c->encrypt($plain_text);

printf qq{encrypt: %s (hex)\n}, unpack 'H*', $cipher_text;
