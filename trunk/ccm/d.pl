#! /usr/bin/perl

use Crypt::CCM;
use strict;

my $key             = pack 'H*', 'C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF';
my $nonce           = pack 'H*', '00000003020100A0A1A2A3A4A5';
my $associated_data = pack 'H*', '0001020304050607';
my $cipher_text     = pack 'H*', '588c979a61c663d2f066d0c2c0f989806d5f6b61dac384509da654e32deac369c2dae7133cb08d';
my $c = Crypt::CCM->new(
    -key => $key,
    -cipher => 'Crypt::Rijndael'
);
$c->set_nonce($nonce);
$c->set_aad($associated_data);
my $plain_text = $c->decrypt($cipher_text);
printf qq{decrypt: %s\n}, $plain_text;
