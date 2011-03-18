<?php 

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
?>