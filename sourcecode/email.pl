#!/usr/bin/perl
use strict;
use warnings;
# first, create your message
use Email::MIME;

my $num_args = $#ARGV + 1;
if ($num_args < 3){
	exit;
}

my $message = Email::MIME->create(
  header_str => [
    From    => $ARGV[0],
    To      => $ARGV[1],
    Subject => "You've been invited.",
  ],
  attributes => {
    encoding => 'quoted-printable',
    charset  => 'ISO-8859-1',
  },
  body_str => $ARGV[2],
);

# send the message
use Email::Sender::Simple qw(sendmail);
sendmail($message);