#include "enc_then_hmac.h"

#include "../general.h"

#define MSG_LEN 64

//@ ATTACKER_PRE(enc_then_hmac)

void *attacker_t(void* data) //@ : pthread_run_joinable
  //@ requires pthread_run_pre(attacker_t)(data, ?info);
  //@ ensures  false;
{
  while(true)
    //@ invariant pthread_run_pre(attacker_t)(data, info);
  {
    //@ open pthread_run_pre(attacker_t)(data, info);
    //@ close enc_then_hmac_proof_pred();
    attacker();
    //@ open enc_then_hmac_proof_pred();
    //@ close pthread_run_pre(attacker_t)(data, info);
  }
   
  return 0;
}

struct enc_then_hmac_args
{
  //@ int sender;
  //@ int receiver;
  
  char* enc_key;
  char* hmac_key;
  char* msg;
  
  int length;
};

/*@

predicate_family_instance pthread_run_pre(sender_t)(void *data, any info) =
  [_]public_invar(enc_then_hmac_pub) &*&
  enc_then_hmac_args_sender(data, ?sender) &*&
  enc_then_hmac_args_receiver(data, ?receiver) &*&
  enc_then_hmac_args_enc_key(data, ?enc_key) &*&
  enc_then_hmac_args_hmac_key(data, ?hmac_key) &*&
  enc_then_hmac_args_msg(data, ?msg) &*& 
  !bad(sender) && !bad(receiver) &*&
  principal(sender, _) &*&
  [1/2]cryptogram(enc_key, KEY_SIZE, ?enc_key_cs, ?enc_key_cg) &*&
    enc_key_cg == cg_symmetric_key(sender, ?enc_id) &*&
    receiver == shared_with(sender, enc_id) &*&
  [1/2]cryptogram(hmac_key, KEY_SIZE, ?hmac_key_cs, ?hmac_key_cg) &*&
    hmac_key_cg == cg_symmetric_key(sender, ?hmac_id) &*&
    receiver == shared_with(sender, hmac_id) &*&
    cg_info(hmac_key_cg) == enc_id &*&
  crypto_chars(secret, msg, MSG_LEN, ?msg_cs) &*&
    true == send(sender, receiver, msg_cs) &*&
  info == IV(sender, IV(receiver, PV(enc_key, CL(enc_key_cs, IV(enc_id, 
             PV(hmac_key, CL(hmac_key_cs, IV(hmac_id, PV(msg, CL(msg_cs, 
                nil))))))))));

predicate_family_instance pthread_run_post(sender_t)(void *data, any info) =
  [_]public_invar(enc_then_hmac_pub) &*&
  enc_then_hmac_args_sender(data, ?sender) &*&
  enc_then_hmac_args_receiver(data, ?receiver) &*&
  enc_then_hmac_args_enc_key(data, ?enc_key) &*&
  enc_then_hmac_args_hmac_key(data, ?hmac_key) &*&
  enc_then_hmac_args_msg(data, ?msg) &*& 
  principal(sender, _) &*&
  [1/2]cryptogram(enc_key, KEY_SIZE, ?enc_key_cs, ?enc_key_cg) &*&
    enc_key_cg == cg_symmetric_key(sender, ?enc_id) &*&
    receiver == shared_with(sender, enc_id) &*&
  [1/2]cryptogram(hmac_key, KEY_SIZE, ?hmac_key_cs, ?hmac_key_cg) &*&
    hmac_key_cg == cg_symmetric_key(sender, ?hmac_id) &*&
    receiver == shared_with(sender, hmac_id) &*&
  crypto_chars(secret, msg, MSG_LEN, ?msg_cs) &*&
  info == IV(sender, IV(receiver, PV(enc_key, CL(enc_key_cs, IV(enc_id, 
             PV(hmac_key, CL(hmac_key_cs, IV(hmac_id, PV(msg, CL(msg_cs, 
                nil))))))))));

@*/

void *sender_t(void* data) //@ : pthread_run_joinable
  //@ requires pthread_run_pre(sender_t)(data, ?x);
  //@ ensures pthread_run_post(sender_t)(data, x) &*& result == 0;
{
  struct enc_then_hmac_args *args = data;
  //@ open pthread_run_pre(sender_t)(data, _);
  sender(args->enc_key, args->hmac_key, args->msg, MSG_LEN);
  //@ close pthread_run_post(sender_t)(data, _);
  return 0;
}

/*@

predicate_family_instance pthread_run_pre(receiver_t)(void *data, any info) =
  [_]public_invar(enc_then_hmac_pub) &*&
  [_]decryption_key_classifier(enc_then_hmac_public_key) &*&
  enc_then_hmac_args_sender(data, ?sender) &*&
  enc_then_hmac_args_receiver(data, ?receiver) &*&
  enc_then_hmac_args_enc_key(data, ?enc_key) &*&
  enc_then_hmac_args_hmac_key(data, ?hmac_key) &*&
  enc_then_hmac_args_msg(data, ?msg) &*&
  enc_then_hmac_args_length(data, _) &*&
  !bad(sender) && !bad(receiver) &*&
  principal(receiver, _) &*&
  [1/2]cryptogram(enc_key, KEY_SIZE, ?enc_key_cs, ?enc_key_cg) &*&
    enc_key_cg == cg_symmetric_key(sender, ?enc_id) &*&
    receiver == shared_with(sender, enc_id) &*&
  [1/2]cryptogram(hmac_key, KEY_SIZE, ?hmac_key_cs, ?hmac_key_cg) &*&
    hmac_key_cg == cg_symmetric_key(sender, ?hmac_id) &*&
    receiver == shared_with(sender, hmac_id) &*&
    cg_info(hmac_key_cg) == enc_id &*&
  chars(msg, MAX_SIZE, _) &*&
  info == IV(sender, IV(receiver, PV(enc_key, CL(enc_key_cs, IV(enc_id, 
             PV(hmac_key, CL(hmac_key_cs, IV(hmac_id, PV(msg, nil)))))))));

predicate_family_instance pthread_run_post(receiver_t)(void *data, any info) =
  enc_then_hmac_args_sender(data, ?sender) &*&
  enc_then_hmac_args_receiver(data, ?receiver) &*&
  enc_then_hmac_args_enc_key(data, ?enc_key) &*&
  enc_then_hmac_args_hmac_key(data, ?hmac_key) &*&
  enc_then_hmac_args_msg(data, ?msg) &*&
  enc_then_hmac_args_length(data, ?length) &*& 
  principal(receiver, _) &*&
  [1/2]cryptogram(enc_key, KEY_SIZE, ?enc_key_cs, ?enc_key_cg) &*&
    enc_key_cg == cg_symmetric_key(sender, ?enc_id) &*&
    receiver == shared_with(sender, enc_id) &*&
  [1/2]cryptogram(hmac_key, KEY_SIZE, ?hmac_key_cs, ?hmac_key_cg) &*&
    hmac_key_cg == cg_symmetric_key(sender, ?hmac_id) &*&
    receiver == shared_with(sender, hmac_id) &*&
  crypto_chars(_, msg, length, ?msg_cs) &*&
  chars(msg + length, MAX_SIZE - length, _) &*&
  col || send(sender, receiver, msg_cs) &*&
  info == IV(sender, IV(receiver, PV(enc_key, CL(enc_key_cs, IV(enc_id, 
             PV(hmac_key, CL(hmac_key_cs, IV(hmac_id, PV(msg, nil)))))))));

@*/

void *receiver_t(void* data) //@ : pthread_run_joinable
  //@ requires pthread_run_pre(receiver_t)(data, ?x);
  //@ ensures pthread_run_post(receiver_t)(data, x) &*& result == 0;
{
  struct enc_then_hmac_args *args = data;
  //@ open pthread_run_pre(receiver_t)(data, _);
  args->length = receiver(args->enc_key, args->hmac_key, args->msg);
  //@ close pthread_run_post(receiver_t)(data, _);
  return 0;
}

int main(int argc, char **argv) //@ : main_full(main_app)
    //@ requires module(main_app, true);
    //@ ensures true;
{
  pthread_t a_thread;
  havege_state havege_state;
  
  printf("\n\tExecuting \"");
  printf("enc_then_hmac protocol");
  printf("\" ... \n\n");
  
  //@ PROTOCOL_INIT(enc_then_hmac)
  //@ int attacker = principal_create();
  //@ int sender = principal_create();
  //@ int receiver = principal_create();
  //@ assume (!bad(sender) && !bad(receiver));
  
  //@ close havege_state(&havege_state);
  havege_init(&havege_state);
  //@ close pthread_run_pre(attacker_t)(NULL, nil);
  pthread_create(&a_thread, NULL, &attacker_t, NULL);
  
  int i = 0;
#ifdef EXECUTE
  while (i++ < 10)
#else
  while (true)
#endif
    /*@ invariant [_]public_invar(enc_then_hmac_pub) &*&
                  [_]decryption_key_classifier(enc_then_hmac_public_key) &*&
                  !bad(sender) && !bad(receiver) &*&
                  havege_state_initialized(&havege_state) &*&
                  principal(sender, ?count) &*& principal(receiver, _);
    @*/
  {
    char* enc_key;
    char* hmac_key;
    int temp;
    
    enc_key = malloc(KEY_SIZE);
    if (enc_key == 0) abort();
    hmac_key = malloc(KEY_SIZE);
    if (hmac_key == 0) abort();
    //@ open principal(sender, _);
    //@ close random_request(sender, 0, true);
    if (havege_random(&havege_state, enc_key, KEY_SIZE) != 0) abort();
    //@ assume (shared_with(sender, count + 1) == receiver);
    //@ assert cryptogram(enc_key, KEY_SIZE, ?enc_cs_key, ?enc_cg_key);
    //@ assert enc_cg_key == cg_symmetric_key(sender, count + 1);
    
    //@ close random_request(sender, count + 1, true);
    if (havege_random(&havege_state, hmac_key, KEY_SIZE) != 0) abort();
    //@ assume (shared_with(sender, count + 2) == receiver);
    //@ assert cryptogram(hmac_key, KEY_SIZE, ?hmac_cs_key, ?hmac_cg_key);
    //@ assert hmac_cg_key == cg_symmetric_key(sender, count + 2);
    {
      pthread_t s_thread, r_thread;

      struct enc_then_hmac_args s_args;
      struct enc_then_hmac_args r_args;
      char s_message[MSG_LEN];
      char r_message[MAX_SIZE];
    
      //@ assert chars(s_message, MSG_LEN, ?msg_cs);
      //@ public_chars(s_message, MSG_LEN);
      //@ chars_to_secret_crypto_chars(s_message, MSG_LEN);
      //@ s_args.sender = sender;
      //@ s_args.receiver = receiver;
      s_args.enc_key = enc_key;
      s_args.hmac_key = hmac_key;
      s_args.msg = s_message;
      
      //@ assume (send(sender, receiver, msg_cs) == true);      
      //@ r_args.sender = sender;
      //@ r_args.receiver = receiver;
      r_args.enc_key = enc_key;
      r_args.hmac_key = hmac_key;
      r_args.msg = r_message;
      
      //@ close principal(sender, _);
      //@ close pthread_run_pre(sender_t)(&s_args, ?s_data);
      //@ close pthread_run_pre(receiver_t)(&r_args, ?r_data);
      pthread_create(&r_thread, NULL, &receiver_t, &r_args);
      pthread_create(&s_thread, NULL, &sender_t, &s_args);
      
      pthread_join(s_thread, NULL);
      //@ open pthread_run_post(sender_t)(&s_args, s_data);
      pthread_join(r_thread, NULL);
      //@ open pthread_run_post(receiver_t)(&r_args, r_data);  
      //@ open [1/2]cryptogram(enc_key, KEY_SIZE, enc_cs_key, _);
      //@ open [1/2]cryptogram(enc_key, KEY_SIZE, enc_cs_key, _);
      //@ open [1/2]cryptogram(hmac_key, KEY_SIZE, hmac_cs_key, _);
      //@ open [1/2]cryptogram(hmac_key, KEY_SIZE, hmac_cs_key, _);

      if (r_args.length != MSG_LEN)
        abort();
      //@ public_crypto_chars(s_message, MSG_LEN);
      //@ chars_to_crypto_chars(s_message, MSG_LEN);
      //@ open principal(sender, _);
      if (memcmp(s_message, r_message, MSG_LEN) != 0)
        abort();
      //@ close principal(sender, _);
      zeroize(r_message, r_args.length);
    }
    zeroize(enc_key, KEY_SIZE);
    free((void*) enc_key);
    zeroize(hmac_key, KEY_SIZE);
    free((void*) hmac_key);
    
    printf(" |%i| ", i);
  }
  
  printf("\n\n\t\tDone\n");
  return 0;
}

