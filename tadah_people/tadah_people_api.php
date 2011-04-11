<?php 
require_once drupal_get_path("module","tadah_people")."/tadah_people_api.php";

function tadah_people_get_profile($uid = NULL) {
  if(empty($uid)) {
    global $user;
    $uid = $user->$uid;
  }
  
  $profile = db_select('tadah_people', 'p')
    ->fields('p')
    ->condition('deleted', 0)
    ->condition('uid', $uid)
    ->execute()
    ->fetchAssoc();
  return $profile;
}

function tadah_people_get_expiring_record($uid = null) {
  if(empty($uid)) {
    global $user;
    $uid = $user->uid;
  }
  
  $record = db_select('tadah_expire', 'e')
    ->fields('e', array('amount', 'expire_date','id'))
    ->condition('uid', $uid)
    ->execute()
    ->fetchAllAssoc('id');
  
  return $record;
}


function tadah_people_get_list($offset = NULL, $limit = NULL, $match = FALSE) {
  
  $offset = empty($offset) ? 0 : $offset;
  $limit = empty($offset) ? 10 : $limit;
  $people =  db_select('tadah_people', 'p')
    ->fields('p', array('uid', 'nick_name', 'surprising_index', 'interests', 'aboutme'))
    ->condition('deleted', 0)
    ->range($offset, $limit)
    ->execute()
    ->fetchAllAssoc('uid');
  return $people;
}

function tadah_people_makeup($uid) {
	$user = user_load($uid);
	$profile = tadah_people_get_profile($uid);
	
	$hash = md5( strtolower( trim( $user->mail ) ) );
	$markup = '<table><tr><td><img src="http://www.gravatar.com/avatar/'.$hash.'?s=160" width="160px"/></td>';
	$markup .= '<td><table><tr><td width="100px"> Nick Name</td><td>'.$profile['nick_name'].'</td></tr>';
	$markup .= '<tr><td>Location</td><td>'.$profile['city'].' - '.$profile['state']. ' - '.$profile['country'].'</td></tr>';
	$markup .= '<tr><td>About Me</td><td>'.$profile['aboutme'].'</td></tr>';
	$markup .= '<tr><td>Interests</td><td>'.$profile['interests'].'</td></tr></table></td>';
	$markup .= '</tr></table>';
	
	return $markup;
}

function tadah_people_send_gift($gift_id, $sender, $receiver, $message = NULL) {
	 
	$gifting_id = db_insert('tadah_gifting')
  ->fields(array(
    'gift_id' => $gift_id,
    'sender' => $sender,
  	'receiver' => $receiver,
  	'message' => $message,
  	'timestamp' => time(),
  ))
  ->execute();
  
  if(!$gifting_id) return 0;
  //expire receiver deposit
  $gift = tadah_gift_load($gift_id);
  $res = db_insert('tadah_expire')
  ->fields(array(
  	'uid' => $receiver,
  	'amount' => $gift->price,
  	'expire_date' => time() + (30 * 24 * 60 * 60),
  	'reciprocating_gifting_id' => $gifting_id,
  ))
  ->execute();
  
  $people = tadah_people_load($sender);
  //deduct sender balance
  $res &= db_update('tadah_people')
  ->fields(array(
  	'deposit' => $people->deposit - $gift->price,
  ))
  ->condition('uid', $people->uid)
  ->execute();
 
  return $res;
}


function debug_base($array, $str=NULL){
  if(true){
    drupal_set_message("<pre>" . " (" . $str . ") " . print_r($array, true). "</pre>");
  }
}

?>