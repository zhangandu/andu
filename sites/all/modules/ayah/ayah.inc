<?php

/**
 * @file
 * General AYAH functionality and helper functions.
 */

/**
 * Helper function for adding/updating a AYAH form.
 *
 * @param $form_id
 *   The form ID to configure.
 * @param $add_game
 *   1 add | 0 don't add the game to the form.
 * @param $delete_form
 *   It deletes the form from the list of forms if true.
 *
 * @return null
 *   Nothing returned.
 *
 */
function ayah_set_form_id_setting($form_id, $add_game, $delete_form = FALSE) {
  if ($delete_form) {
    db_query("DELETE FROM {ayah_forms} WHERE form_id = :form_id", array('form_id' => $form_id));
  }
  else{
    $sql = "INSERT INTO {ayah_forms} (form_id, add_game) VALUES (:form_id, :add_game) ON DUPLICATE KEY UPDATE add_game= :add_game;";
    db_query($sql, array(':form_id' => $form_id, ':add_game' => $add_game, ':add_game' => $add_game));
  }
}


/**
 * Get the AYAH setting for a given form_id.
 *
 * @param $form_id
 *   The form_id to query for
 *
 * @return NULL
 *   If no setting is known or a ayah_forms object with fields 'add_game'.
 *
 */
function ayah_get_form_id_setting($form_id) {
  $result = db_query("SELECT add_game FROM {ayah_forms} WHERE form_id = :form_id", array(':form_id' => $form_id));
  if (!$result) {
    return NULL;
  }
  $ayah_form = $result->fetchObject();
  return $ayah_form;
}


/**
 * Helper function to get placement information for a given form_id.
 *
 * @param $form_id
 *   The form_id to get the placement information for.
 * @param $form
 *   If a form corresponding to the given form_id, if there
 *   is no placement info for the given form_id, this form is examined to
 *   guess the placement.
 *
 * @return array
 *   Placement info array.
 */
function _ayah_get_ayah_placement($form_id, $form) {
  // Get AYAH placement map from cache. Two levels of cache:
  // static variable in this function and storage in the variables table.
  static $placement_map = NULL;
  // Try first level cache.
  if ($placement_map === NULL) {
    // If first level cache missed: try second level cache.
    $placement_map = variable_get('ayah_placement_map_cache', NULL);

    if ($placement_map === NULL) {
      // If second level cache missed: start from a fresh placement map.
      $placement_map = array();
      // Prefill with some hard coded default entries.
      //
      // The comment form can have a 'Preview' button, or both a 'Submit'
      // and 'Preview' button, which is tricky for automatic placement
      // detection. Luckily, Drupal core sets their weight (19 and 20),
      // so we just have to specify a slightly smaller weight.
      $placement_map['comment_form'] = array('path' => array(), 'key' => NULL, 'weight' => 18.9);
      // Additional note: the node forms also have the posibility to only
      // show a 'Preview' button. However, the 'Submit' button is always
      // present, but is just not rendered ('#access' = FALSE) in those cases.
      // The the automatic button detection should be sufficient for node forms.
      //
      // TODO: also make the placement 'overridable' from the admin UI?
    }
  }
  // Query the placement map.
  if (array_key_exists($form_id, $placement_map)) {
    $placement = $placement_map[$form_id];
  }
  // If no placement info is available in placement map:
  // search the form for buttons and guess placement from it.
  else {
    $buttons = _ayah_search_buttons($form);
    if (count($buttons)) {
      // Pick first button.
      // TODO: make this more sophisticated? Use cases needed.
      $placement = $buttons[0];
    }
    else {
      // Use NULL when no buttons were found.
      $placement = NULL;
    }
    // Store calculated placement in caches.
    $placement_map[$form_id] = $placement;
    variable_set('ayah_placement_map_cache', $placement_map);
  }

  return $placement;
}

/**
 * Helper function for searching the buttons in a form.
 *
 * @param $form
 *   The form to search button elements in.
 *
 * @return array
 *   An array of paths to the buttons.
 *   A path is an array of keys leading to the button, the last
 *   item in the path is the weight of the button element
 *   (or NULL if undefined).
 *
 */
function _ayah_search_buttons($form) {
  $buttons = array();
  foreach (element_children($form) as $key) {
    // Look for submit or button type elements.
    if (isset($form[$key]['#type']) && ($form[$key]['#type'] == 'submit' || $form[$key]['#type'] == 'button')) {
      $weight = isset($form[$key]['#weight']) ? $form[$key]['#weight'] : NULL;
      $buttons[] = array(
        'path' => array(),
        'key' => $key,
        'weight' => $weight,
      );
    }
    // Process children recurively.
    $children_buttons = _ayah_search_buttons($form[$key]);
    foreach ($children_buttons as $b) {
      $b['path'] = array_merge(array($key), $b['path']);
      $buttons[] = $b;
    }
  }
  return $buttons;
}


/**
 * Helper function to insert a AYAH game element in a form before a given
 * form element.
 *
 * @param $form
 *   The form to add the AYAH element to.
 * @param $placement
 *   Information where the AYAH element should be inserted.
 *   $placement should be an associative array with fields:
 *     - 'path': path (array of path items) of the container in the form where
 *       the AYAH element should be inserted.
 *     - 'key': the key of the element before which the AYAH element
 *       should be inserted. If the field 'key' is undefined or NULL, the AYAH
 *       will just be appended to the container.
 *     - 'weight': if 'key' is not NULL: should be the weight of the element
 *       defined by 'key'. If 'key' is NULL and weight is not NULL: set the
 *       weight property of the AYAH element to this value.
 * @param $ayah_element
 *   The AYAH element to insert.
 *
 */
function _ayah_insert_game_element(&$form, $placement, $ayah_element) {
  // Get path, target and target weight or use defaults if not available.
  $target_key = isset($placement['key']) ? $placement['key'] : NULL;
  $target_weight = isset($placement['weight']) ? $placement['weight'] : NULL;
  $path = isset($placement['path']) ? $placement['path'] : array();

  // Walk through the form along the path.
  $form_stepper = &$form;
  foreach ($path as $step) {
    if (isset($form_stepper[$step])) {
      $form_stepper = & $form_stepper[$step];
    }
    else {
      // Given path is invalid: stop stepping and
      // continue in best effort (append instead of insert).
      $target_key = NULL;
      break;
    }
  }

  // If no target is available: just append the AYAH element to the container.
  if ($target_key == NULL || !array_key_exists($target_key, $form_stepper)) {
    // Optionally, set weight of AYAH element.
    if ($target_weight != NULL) {
      $ayah_element['#weight'] = $target_weight;
    }
    $form_stepper['ayah'] = $ayah_element;
  }
  // If there is a target available: make sure the AYAH element comes
  // right before it.
  else {
    // If target has a weight: set weight of AYAH element a bit smaller
    // and just append the AYAH: sorting will fix the ordering anyway.
    if ($target_weight != NULL) {
      $ayah_element['#weight'] = $target_weight - .1;
      $form_stepper['ayah'] = $ayah_element;
    }
    else {
      // If we can't play with weights: insert the AYAH element at the right
      // position. Because PHP lacks a function for this (array_splice() comes
      // close, but it does not preserve the key of the inserted element),
      // we do it by hand: chop of the end, append the AYAH element and put
      // the end back.
      $offset = array_search($target_key, array_keys($form_stepper));
      $end = array_splice($form_stepper, $offset);
      $form_stepper['ayah'] = $ayah_element;
      foreach ($end as $k => $v) {
        $form_stepper[$k] = $v;
      }
    }
  }
}


/**
 * Internal:
 * Makes a call with CURL
 */
function _ayah_doCall($url, $fields, $use_timeout = FALSE) {
  $fields_string = "";
  foreach ($fields as $key => $value) {
    if (is_array($value)) {
      foreach ($value as $v) {
        $fields_string .= $key . '[]=' . $v . '&';
      }
    }
    else{
      $fields_string .= $key . '=' . $value . '&';
    }
  }
  rtrim($fields_string, '&');
  $curlsession = curl_init();
  curl_setopt($curlsession, CURLOPT_URL, $url);
  curl_setopt($curlsession, CURLOPT_POST, count($fields));
  curl_setopt($curlsession, CURLOPT_POSTFIELDS, $fields_string);
  curl_setopt($curlsession, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($curlsession, CURLOPT_SSL_VERIFYHOST, 0);
  curl_setopt($curlsession, CURLOPT_SSL_VERIFYPEER, FALSE);
  if ($use_timeout) {
    curl_setopt($curlsession, CURLOPT_TIMEOUT, $this->timeout);
  }
  $result = curl_exec($curlsession);

  if ($result) {
    try {
      $m = json_decode($result);
    } catch (Exception $e) {
      error_log("AYAH::doCall() - Exception when calling json_decode: " . $e->getMessage());
      $m = NULL;
    }
  }
  else{
    $m = NULL;
  }
  curl_close($curlsession);
  return $m;
}
