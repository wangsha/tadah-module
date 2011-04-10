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

function debug_base($array, $str=NULL){
  if(true){
    drupal_set_message("<pre>" . " (" . $str . ") " . print_r($array, true). "</pre>");
  }
}

?>