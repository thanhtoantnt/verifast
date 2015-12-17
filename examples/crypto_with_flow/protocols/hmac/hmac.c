#include "hmac.h"

#include <stdlib.h>
#include <string.h>

//@ #include "quantifiers.gh"

#define SERVER_PORT 121212

void sender(char *key, int key_len, char *message)
  /*@ requires [_]public_invar(hmac_pub) &*&
               principal(?sender, _) &*&
               [?f1]cryptogram(key, key_len, ?key_cs, ?key_cg) &*&
                 key_cg == cg_symmetric_key(sender, ?id) &*&
               [?f2]chars(message, MESSAGE_SIZE, ?msg_cs) &*&
               true == send(sender, shared_with(sender, id), msg_cs); @*/
  /*@ ensures  principal(sender, _) &*&
               [f1]cryptogram(key, key_len, key_cs, key_cg) &*&
               [f2]chars(message, MESSAGE_SIZE, msg_cs); @*/
{
  int socket;
  char hmac[64];
  
  net_usleep(20000);
  if(net_connect(&socket, NULL, SERVER_PORT) != 0)
    abort();
  if(net_set_block(socket) != 0)
    abort();
  
  {
    int message_len = MESSAGE_SIZE + 64;
    char* M = malloc(message_len);
    if (M == 0) abort();
    
    //@ chars_to_crypto_chars(message, MESSAGE_SIZE);
    memcpy(M, message, MESSAGE_SIZE);
    sha512_hmac(key, (unsigned int) key_len, M, 
                (unsigned int) MESSAGE_SIZE, hmac, 0);
    //@ assert cryptogram(hmac, 64, ?hmac_cs, ?hmac_cg);
    //@ close hmac_pub(hmac_cg);
    //@ leak hmac_pub(hmac_cg);
    //@ public_cryptogram(hmac, hmac_cg);
    //@ chars_to_crypto_chars(hmac, 64);
    memcpy(M + MESSAGE_SIZE, hmac, 64);
    
    //@ crypto_chars_to_chars(M, MESSAGE_SIZE);
    //@ crypto_chars_to_chars(M + MESSAGE_SIZE, 64);
    //@ chars_join(M);
    net_send(&socket, M, (unsigned int) message_len);
    free(M);
  }
  net_close(socket);
}

void receiver(char *key, int key_len, char *message)
  /*@ requires [_]public_invar(hmac_pub) &*&
               principal(?receiver, _) &*&
               [?f1]cryptogram(key, key_len, ?key_cs, ?key_cg) &*&
                 key_cg == cg_symmetric_key(?sender, ?id) &*&
                 receiver == shared_with(sender, id) &*&
               chars(message, MESSAGE_SIZE, _); @*/
  /*@ ensures  principal(receiver, _) &*&
               [f1]cryptogram(key, key_len, key_cs, key_cg) &*&
               chars(message, MESSAGE_SIZE, ?msg_cs) &*&
               col || bad(sender) || bad(receiver) ||
               send(sender, receiver, msg_cs); @*/
{
  int socket1;
  int socket2;
  
  if(net_bind(&socket1, NULL, SERVER_PORT) != 0)
    abort();
  if(net_accept(socket1, &socket2, NULL) != 0)
    abort();
  if(net_set_block(socket2) != 0)
    abort();
  
  {
    int size;
    char buffer[MAX_MESSAGE_SIZE];
    char hmac[64];
  
    size = net_recv(&socket2, buffer, MAX_MESSAGE_SIZE);
    int expected_size = MESSAGE_SIZE + 64;
    if (size != expected_size) abort();
    //@ chars_split(buffer, expected_size);
    //@ assert chars(buffer, MESSAGE_SIZE, ?msg_cs);
    /*@ close hide_chars((void*) buffer + expected_size, 
                         MAX_MESSAGE_SIZE - expected_size, _); @*/
    
    //Verify the hmac
    //@ chars_to_crypto_chars(buffer, MESSAGE_SIZE);
    sha512_hmac(key, (unsigned int) key_len, buffer, 
                (unsigned int) MESSAGE_SIZE, hmac, 0);
    memcpy(message, (void*) buffer , MESSAGE_SIZE);
    //@ open cryptogram(hmac, 64, ?hmac_cs, ?hmac_cg);
    //@ assert hmac_cg == cg_hmac(sender, id, msg_cs);
    //@ assert chars((void*) buffer + MESSAGE_SIZE, 64, ?hmac_cs2);
    //@ chars_to_crypto_chars((void*) buffer + MESSAGE_SIZE, 64);
    if (memcmp((void*) buffer + MESSAGE_SIZE, hmac, 64) != 0) abort();
    //@ crypto_chars_join(buffer);
    //@ crypto_chars_to_chars(buffer, expected_size);
    
    /*@ if (!col && !bad(sender) && !bad(receiver))
        {
          public_chars(hmac, 64);
          public_chars_extract(hmac, hmac_cg);
          open [_]hmac_pub(hmac_cg);
          assert (send(sender, receiver, msg_cs) == true); 
        }
    @*/
    /*@ open hide_chars((void*) buffer + expected_size, 
                        MAX_MESSAGE_SIZE - expected_size, _); @*/
  }
  net_close(socket2);
  net_close(socket1);
}