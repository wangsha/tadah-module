<?php

function tadah_gift_select_form($form, &$form_state) {
	global $user;
	if(!$user->uid) {
		drupal_set_title('Welcome to Tadah Gift!');
		$form['markup'] = array(
		  '#type' => 'markup',
			'#markup' => '<b>Please login to start</b>',
		);
		return $form;
	}
	$result = db_query_range('SELECT * FROM {tadah_gift} WHERE 1', 0, 10);
	$rows = array();
	foreach ($result as $gift) {
		$row = array();
		$id = $gift->gift_id;
		$link = "tadah_people/select/".$id;
		$form['gift_'.$id] = array(
		  '#type' => 'hidden',
			'#value' => $id,
		);
		
		$form['image_'.$id] = array(
			'#type' => 'markup',
		  '#markup' => '<a href="'.$gift->detail_url.'"><img src="'.$gift->picture_url.'" alt="'.$gift->name.'" width="160px"></a>'
		);
		$row['image_'.$id] = drupal_render($form['image_'.$id]);
		$row['info_'.$id] = '<b>'.l($gift->name, $gift->detail_url).'</b>';
		$row['info_'.$id] .= '<br>'.$gift->description;
		$row['price_'.$id] = $gift->price;
		$row['surprise_index_'.$id] = $gift->surprise_index;
		$row['select_'.$id] = '<b>' . l('Send!', $link) . '</b>';
		$rows[] = $row;
	}
	
	$table = theme_table(array(
	  'rows' => $rows,
		'attributes' => array(),
		'header' => array('Image', 'Information', 'Price', 'Surprising Index', 'Send!'),
		'caption' => '',
		'colgroups' => array(),
		'sticky' => True,
		'empty' => 'No Gift Available. '.l('Add a Gift!', 'tadah_gift/add'),
	));

	$form['result_table'] = array(
	  '#type' => 'markup',
		'#markup' => $table,
	);
	
	 
	
	return $form;
}
?>