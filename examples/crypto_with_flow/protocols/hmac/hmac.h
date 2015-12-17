#ifndef HMAC_H
#define HMAC_H

#include "../../annotated_api/polarssl_definitions/polarssl_definitions.h"

#define MESSAGE_SIZE 40

/*@

// 1. sender -> receiver : {M, HMAC(K, M)}

///////////////////////////////////////////////////////////////////////////////
// Encodings for this protocol ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

fixpoint int shared_with(int sender, int id);
fixpoint bool send(int sender, int receiver, list<char> message);

///////////////////////////////////////////////////////////////////////////////
// Definition of pub for this protocol ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

fixpoint bool hmac_public_key(int p, int c)
{
  return bad(p) || bad(shared_with(p, c));
}

predicate hmac_pub(cryptogram cg) =
  switch (cg)
  {
    case cg_nonce(p0, c0):
      return true;
    case cg_symmetric_key(p0, c0):
      return true == hmac_public_key(p0, c0);
    case cg_public_key(p0, c0):
      return true;
    case cg_private_key(p0, c0):
      return true == bad(p0);
    case cg_hash(cs0):
      return true;
    case cg_hmac(p0, c0, cs0):
      return hmac_public_key(p0, c0) ||
             send(p0, shared_with(p0, c0), cs0);
    case cg_encrypted(p0, c0, cs0, ent0):
      return true == hmac_public_key(p0, c0) &*&
             [_]public_generated(hmac_pub)(cs0);
    case cg_auth_encrypted(p0, c0, mac0, cs0, ent0):
      return true == hmac_public_key(p0, c0) &*&
             [_]public_generated(hmac_pub)(cs0);
    case cg_asym_encrypted(p0, c0, cs0, ent0):
      return [_]public_generated(hmac_pub)(cs0);
    case cg_asym_signature(p0, c0, cs0, ent0):
      return true == bad(p0);
  }
;

@*/

///////////////////////////////////////////////////////////////////////////////
// Implementation prototypes for this protocol ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void sender(char *key, int key_len, char *message);
  /*@ requires [_]public_invar(hmac_pub) &*&
               principal(?sender, _) &*&
               [?f1]cryptogram(key, key_len, ?key_cs, ?key_cg) &*&
                 key_cg == cg_symmetric_key(sender, ?id) &*&
               [?f2]chars(message, MESSAGE_SIZE, ?msg_cs) &*&
               true == send(sender, shared_with(sender, id), msg_cs); @*/
  /*@ ensures  principal(sender, _) &*&
               [f1]cryptogram(key, key_len, key_cs, key_cg) &*&
               [f2]chars(message, MESSAGE_SIZE, msg_cs); @*/

void receiver(char *key, int key_len, char *message);
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

///////////////////////////////////////////////////////////////////////////////
// Attacker proof obligations for this protocol ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//@ PUBLIC_INVARIANT_PROOFS(hmac)

#endif