<?php
require_once drupal_get_path("module","tadah_people")."/tadah_people_api.php";

function tadah_people_select_form($form, &$form_state, $gift_id) {
	$form = array();
	$markup = tadah_gift_markup($gift_id);

	$form['gift_info'] = array(
	  '#type' => 'markup',
		'#markup' => $markup,
	);

	global $user;
	$uid = $user->uid;
	$result = db_query_range('SELECT uid, nick_name, city, state, country, aboutme, interests, surprising_index
	FROM {tadah_people} WHERE uid<> :uid', 0, 10, array('uid' => $uid));
	$rows = array();
	foreach ($result as $person) {
		$row = array();
		$id = $person->uid;
		$link = "tadah_people/send/".$gift_id.'/'.$person->uid;
		$form['person_'.$id] = array(
		  '#type' => 'hidden',
			'#value' => $id,
		);

		$p = user_load($id);
		$hash = md5( strtolower( trim( $p->mail ) ) );
		$row['name_'.$id] .= '<img src="http://www.gravatar.com/avatar/'.$hash.'?s=60" width="60px"/><br><b>'.$person->nick_name.'</b>';
		$row['location_'.$id] = $person->city . ' - '. $person->state .' - '. $person->country;
		$row['aboutme_'.$id] = $person->aboutme;
		$row['interests_'.$id] = $person->interests;
		$row['surprise_index_'.$id] = $person->surprising_index;
		$row['select_'.$id] = '<b>' . l('Send!', $link) . '</b>';
		$rows[] = $row;
	}
	$table = theme_table(array(
	  'rows' => $rows,
		'attributes' => array(),
		'header' => array('Name', 'Location', 'About Me', 'Interest', 'Surprising Index', 'Send!'),
		'caption' => '',
		'colgroups' => array(),
		'sticky' => True,
		'empty' => 'Oops, nobody is here. :( ',
	));

	$form['result_table'] = array(
	  '#type' => 'markup',
		'#markup' => $table,
		'#prefix' => '<div style="height:30px"></div>',
	);
	return $form;
}

function tadah_people_send_form($form, &$form_state, $gift_id, $person_id) {
	drupal_set_title('Send Gift!');
	$gift_markup = tadah_gift_markup($gift_id);
	$form['gift_info'] = array(
	  '#type' => 'markup',
		'#markup' => $gift_markup,
		'#prefix' => '<b>Gift</b>',
	);

	$form['gift_id'] = array(
		'#type' => 'hidden',
		'#value' => $gift_id,
	);
	$form['person_id'] = array(
		'#type' => 'hidden',
		'#value' => $person_id,
	);
	$people_markup = tadah_people_makeup($person_id);
	$form['person_info'] = array(
	  '#type' => 'markup',
		'#markup' => $people_markup,
		'#prefix' => '<b>People</b>',
	);
	$form['message'] = array(
	  '#type'=>'textarea',
		'#title' => 'Attach a message to the gift:',

	);

	$form['send'] = array(
	  '#type' => 'submit',
		'#value' => 'Send!'
		);
		return $form;
}
function tadah_people_gifting_history($form, &$form_state) {
	global $user;
	$uid = $user->uid;
	list($module,$mode) = explode('/', $_GET['q']);
	if($mode == 'sending') {
		drupal_set_title('Gifts Sent');
		$sql = "SELECT g.name, g.price, g.picture_url, f.timestamp, f.message, f.receiver, p.nick_name, u.mail
	  				FROM tadah_gift as g, tadah_gifting as f, tadah_people as p, users as u
	  				WHERE g.gift_id = f.gift_id AND u.uid = p.uid
	  				AND p.uid = f.sender AND f.sender = :uid ORDER BY f.timestamp DESC";
		//check whether need to send out more gift
	}else{
		drupal_set_title('Gifts Received');
		$sql = "SELECT g.name, g.price, g.picture_url, f.timestamp, f.message, f.sender, p.nick_name, u.mail
	  				FROM tadah_gift as g, tadah_gifting as f, tadah_people as p, users as u
	  				WHERE g.gift_id = f.gift_id AND u.uid = p.uid
	  				AND p.uid = f.sender AND f.receiver = :uid ORDER BY f.timestamp DESC";

	}
	$result = db_query_range($sql, 0, 10, array('uid'=>$uid));
	$rows = array();
	foreach($result as $record) {
		$row = array();
		$row['gift'] = '<img src="'.$record->picture_url.'" width="160px"/>';
		$row['gift'] .= '<br><b>'.$record->name.', USD '.$record->price.'</b>';
		$hash = md5( strtolower( trim( $record->mail ) ) );

		$row['message'] = '<img src="http://www.gravatar.com/avatar/'.$hash.'?s=60" width="60px"/>';
		$row['message'] .= 'To <b>'.$record->nick_name.'</b> at <i>'.date("d M Y", $record->timestamp).'</i>';
		$row['message'] .= '<hr>'.$record->message;
		$rows[] = $row;
	}

	//to send or receive
	//coming soon
	$recv_sql = "SELECT SUM(g.price) as received
					 FROM tadah_gifting f, tadah_gift as g
					 WHERE f.receiver = :uid AND f.gift_id = g.gift_id";
	$sent_sql = "SELECT SUM(g.price) as sent
					 FROM tadah_gifting f, tadah_gift as g
					 WHERE f.sender = :uid AND f.gift_id = g.gift_id";

	$result = db_query($recv_sql, array('uid'=>$uid));
	foreach ($result as $record) {
		$sum_recv = $record->received;
	}

	$result = db_query($sent_sql, array('uid'=>$uid));
	foreach ($result as $record) {
		$sum_sent = $record->sent;
	}
	$markup;
	//more gifts on the way
	if( ($mode == 'receiving') && ($sum_sent > $sum_recv) ) {
		$last_sent_sql = "SELECT max(timestamp) as timestamp FROM tadah_gifting WHERE sender=:uid";
		$result = db_query($last_sent_sql, array('uid' => $uid));
		$timestamp;
		foreach($result as $record) {
			$timestamp = $record->timestamp;
		}
		
		$markup = '<div><div style="float:left"><img src="'.drupal_get_path('module', 'tadah_people').'/gift_on_the_way.jpg" width="200px"/></div>';
		$markup .= '<div style="padding-left:215px"><span style="font-size:2em; font-weight:bold"><i>You have a gift of USD '.($sum_sent-$sum_recv);
		$markup .=' on the way!<br><br> Latest by: '.date('d M Y', $timestamp+30*24*3600).'</i></span>';
		$markup .= '</div></div>';
	}
	if( ($mode == 'sending') && ($sum_sent < $sum_recv) ) {
		$last_recv_sql = "SELECT max(timestamp) as timestamp FROM tadah_gifting WHERE receiver=:uid";
		$result = db_query($last_recv_sql, array('uid' => $uid));
		$timestamp;
		foreach($result as $record) {
			$timestamp = $record->timestamp;
		}
		$expire_date = date('d M Y', $timestamp+30*24*3600);
		$markup = '<div><div style="float:left"><img src="'.drupal_get_path('module', 'tadah_people').'/make_a_gift.png" width="200px"/></div>';
		$markup .='<div style="padding-left:215px"><span style="font-size:1.5em; font-weight:bold;">Your account has <i>USD '.($sum_sent-$sum_recv);
		$markup .='</i> going to expire at <i>'.$expire_date.'<br>'.l('Send a gift now!', 'tadah_gift/select').'</i></span><br>Tadah Gift will send a gift on behalf you when the money expires.</div></div>';

	}
	$form['status'] = array(
			'#type' => 'markup',
		 	'#markup' => $markup,
	);
	$table = theme_table(array(
	  'rows' => $rows,
		'attributes' => array(),
		'caption' => '',
		'header' => array(),
		'colgroups' => array(),
		'sticky' => True,
		'empty' => 'Oops, no record here. :( '. l('Send a Gift Now!', 'tadah_gift/select'),
	));

	$form['result_table'] = array(
	  '#type' => 'markup',
		'#markup' => $table,
	);





	return $form;
}
function tadah_people_send_form_submit($form, &$form_state) {
	$values = $form_state['values'];
	$gift_id = $values['gift_id'];
	$receiver = $values['person_id'];
	$message = $values['message'];

	global $user;
	$sender = $user->uid;
	$res = tadah_people_send_gift($gift_id, $sender, $receiver, $message);

	drupal_set_message('Tadah! Your gift has been sent sucessfully. :)');


}
?>