<?php

/**
 * Class silverpopAPI
 * @author Nathanael Dewhurst (ndewhurst@digitalbungalow.com)
 * @author Erin McLaughlin (emclaughlin@digitalbungalow.com)
 * @date 2013-04-26
 */

class silverpopAPI {

  private $instance_id;
  private $error_list;

  public function __construct() {
    $this->initErrorData();
    // Generate a somewhat unique integer to distinguish this object instance,
    //  for use in caching data.
    $this->instance_id = rand();
    module_load_include('inc', 'silverpop', 'silverpopXML');
    // @todo consider bringing some or all of the account settings from
    //  silverpopXML in here to avoid re-declaring them on every method call
    //  (NRD 2013-04-26)
  }

  /**
   * Store a mapping of Silverpop API error codes and their meanings
   */
  private function initErrorData() {
    // @todo: add the remaining (~1-2 hundred) errors, if this proves
    //  useful (NRD 2013-04-26)
    $this->error_list = array(
      1 => 'FTF_NON_NUMERIC_MAILING_KEY',
      2 => 'FTF_NON_NUMERIC_SENDER_KEY',
      
    );
  }

  /**
   * Convert a numeric code to a useful error message string
   * @param $code
   * @return null|string
   */
  public function translateErrorCode($code) {
    if (array_key_exists($code, $this->error_data)) {
      return $this->error_data[$code];
    }
    else {
      return t('Unspecified Silverpop Error');
    }
  }

  /**
   * Perform a SilverPop API operation
   * @param string $command - The name of an API operation/function
   * @param array $data - An array of data to submit with the API call, where
   *  the structure of the (often multidimensional) array mirrors that of the
   *  XML that will be sent to Silverpop
   * @return array
   */
  private function execute($command, $data = NULL) {
    $dom = new DOMDocument;
    // Create an XML node for the specified command...
    $commandNode = $dom->createElement($command);
    // ...then convert all the associated $data to XML...
    $this->buildXML($data, $dom, $commandNode);
    // ...and place it inside that command node
    $dom->appendChild($commandNode);

    // Submit the formatted XML to SilverPop
    $spXML = new silverpopXML($dom, TRUE);
    $response = $spXML->apicall();
    return $response;
  }

  /**
   * Recursively translate an associative array to a usable XML structure.
   * @param $data
   * @param $dom
   * @param $parent
   */
  private function buildXML($data, &$dom, &$parent) {
    foreach ($data as $key => $value) {
      $newNode = $dom->createElement($key);
      // If $value is an array, it corresponds to a non-leaf XML node, so
      //  create an element for it in the $dom tree, then walk through its
      //  descendants, and then eventually attach it to its parent
      if (is_array($value)) {
        // The "key_name" setting indicates that this array is a series of
        //  values who all use the same XML element name (key_name)
        // @todo: test, review, and possibly rewrite.
        if (array_key_exists('key_name', $value)) {
          $key_name = $value['key_name'];
          unset($value['key_name']);
          foreach ($value as $item) {
            $subNode = $dom->createElement($key_name);
            $this->buildXML($item, $dom, $subNode);
            $newNode->appendChild($subNode);
          }
        }
        // The custom fields represented by the 'COLUMN' XML nodes follow a
        //  special pattern (name/value). To make it interesting, the "COLUMNS"
        //  node appears in the documentation for all(?) methods that use this
        //  node, but its inclusion wreaks havoc on some methods (at least
        //  addRecipient). All methods we've tested appear to work fine without
        //  it (NRD 2013-07-08).
        // @todo make this more generic to handle any element that uses this
        //  pattern, e.g. 'SYNC_FIELDS' (NRD 2013-04-26)
        if ($key == 'COLUMNS') {
          foreach ($value as $column_name => $column_value) {
            $columnNode = $dom->createElement('COLUMN');
            if (is_numeric($column_name)) {
              $column_data = array(
                'NAME' => $column_value,
              );
            }
            else {
              $column_data = array(
                'NAME' => $column_name,
                'VALUE' => $column_value,
              );
            }
            $this->buildXML($column_data, $dom, $columnNode);
            // Attach each 'COLUMN' node directly beneath the parent node
            //  (there is no "COLUMNS" node in the output XML)
            $parent->appendChild($columnNode);
          }
        }
        elseif ($key == 'SUBSTITUTIONS') {
          foreach ($value as $column_name => $column_value) {
            $columnNode = $dom->createElement('SUBSTITUTION');
            // @Review: The trailing "]]>" below was appearing after
            //  the body of our test emails. If this is true, consider
            // escaping/encoding as necessary, and/or check email's HTML for
            //  solitary opening/closing tags/brackets. Alternatively, do not
            //  use the CDATA wrapper (NRD 2013-05-20)
            $column_data = array(
              'NAME' => $column_name,
//              'VALUE' => '<![CDATA[' . $column_value . ']]>',
              'VALUE' => $column_value,
            );
            $this->buildXML($column_data, $dom, $columnNode);
            $newNode->appendChild($columnNode);
          }
        }
        elseif ($key == 'SYNC_FIELDS') {
          foreach ($value as $sync_field_name => $sync_field_value) {
            $syncFieldNode = $dom->createElement('SYNC_FIELD');
            $sync_field_data = array(
              'NAME' => $sync_field_name,
              'VALUE' => $sync_field_value,
            );
            $this->buildXML($sync_field_data, $dom, $syncFieldNode);
            // Attach each 'SYNC_FIELD' node directly beneath the parent node
            //  (there is no "SYNC_FIELDS" node in the output XML)
            // @todo: test this
            $parent->appendChild($syncFieldNode);
          }
        }
        elseif ($key == 'SUPPRESSION_LISTS') {
          foreach ($value as $suppression_list_id) {
            $node_data = array(
              'SUPPRESSION_LIST_ID' => $suppression_list_id,
            );
            $this->buildXML($node_data, $dom, $newNode);
          }
        }
        // In most cases, $value will be an array that follows the normal
        //  pattern, and we will simply pass it to this function recursively
        else {
          $this->buildXML($value, $dom, $newNode);
        }
        $parent->appendChild($newNode);
      }
      // If $value is not an array, it corresponds to a leaf XML node, so create
      //  one container element (node) and then a simple text node therein
      else {
        $childNode = $dom->createTextNode($value);
        $newNode->appendChild($childNode);
      }
      $special_nodes = array(
        'COLUMNS',
//        'SUBSTITUTIONS',
//        'SYNC_FIELDS',
      );
      if (!in_array($key, $special_nodes)) {
        $parent->appendChild($newNode);
      }
    }
  }

  /**
   * Find links within a given content string and ensure that they are
   *  structured such that Silverpop can track them.
   *
   * @param $content
   */
  protected function enableLinkTracking(&$content) {

    global $link_count;
    $link_count = 1;

    $contentHTML = str_get_html($content);

    foreach($contentHTML->find('a') as $linkElement) {

      $link_url = $linkElement->href;

      // Check to see if this link matches one of the patterns identifying links
      //  that shouldn't be tracked.
      $untracked_domains_string = variable_get('silverpop_domains_to_exclude_from_tracking', FALSE);
      if ($untracked_domains_string) {
        $untracked_domains = explode(',', $untracked_domains_string);
      }
      $untracked_domain = FALSE;
      foreach ($untracked_domains as $blacklisted_domain) {
        if (strpos($link_url, trim($blacklisted_domain)) != FALSE) {
          $untracked_domain = TRUE;
          break;
        }
      }
      if ($untracked_domain) {
        // If this link matches one of the "do not track" patterns, then skip
        //  ahead to the next link in the HTML content collection.
        continue;
      }


      $dll_server = variable_get('silverpop_dll_server', FALSE);

      $url_append_string_domains = variable_get('silverpop_url_append_string_domains', FALSE);
      if ($url_append_string_domains) {
        $domains = explode(',', $url_append_string_domains);
      }
      $url_append_string = variable_get('silverpop_url_append_string', FALSE);

      // If the SP administrator has specified that a certain string be
      //  appended to all links, modify the href of the link accordingly.
      //  However, the admin can specify that this should only apply to links
      //  that point to a given set of domains (stored in the
      //  "silverpop_url_append_string_domains" variable).
      if ($url_append_string) {
        // Do not append URL parameters to "mailto" links.
        if (strpos($link_url, 'mailto') === FALSE) {
          $valid_domain = TRUE;
          // If domain-specific append string application is enabled...
          if ($url_append_string_domains) {
            $valid_domain = FALSE;
            // ...check to see if any of the domains in the whitelist appear
            //  in this link's URL (this is a loose match)...
            foreach ($domains as $whitelisted_domain) {
              if (strpos($link_url, trim($whitelisted_domain)) != FALSE) {
                // ...and, if so, declare that we should append the given
                //  string to this link's URL.
                $valid_domain = TRUE;
                break;
              }
            }
          }
          // If this link's domain is on the whitelist for append string
          //  application, or if no domains have been specified, append the
          //  string to the link URL.
          if ($valid_domain) {
            // The link's existing URL may already contain query parameters.
            if (strpos($link_url, '?') != FALSE) {
              // If so, join the provided string using an ampersand to extend
              //  the existing query string.
              $link_url .= '&';
            }
            else {
              // If not, initiate a query string using a question mark.
              $link_url .= '?';
            }
            // Then append the URL with the provided string.
            $link_url .= $url_append_string;
          }
        }
      }

      // If a Dynamic Link Loader server has been specified, rewrite this link
      //  to route its traffic through that server for SP tracking purposes.
      if ($dll_server) {
        // Generate a friendly name for this link, to use in SP reports.
        $lean_contents = preg_replace('/\s+/', ' ', $linkElement->plaintext);
        $trim_contents = trim($lean_contents);
        $short_contents = substr($trim_contents, 0, 64);
        $link_name = urlencode('Link ' . $link_count . ' - ' . $short_contents);

        // Encode ampersands in the original URL, so they are not
        //  interpreted as query parameter delimiters in the containing URL.
        //  Note: we do not want to simply urlencode the entire $link_url
        //  string, because that would encode other chars, e.g. the percent
        //  signs used to designate Silverpop substitution/personalization
        //  tokens (NRD 2013-06-04).
        $encoded_url = preg_replace('/&/', '%26', trim($link_url));

        $href = 'http://' . $dll_server
          . '/ctt?m=%%MAILING_ID%%&r=%%RECIPIENT_ID%%&b=0&j=%%JOB_ID_CODE%%'
          . '&k=' . $link_name
          . '&kx=1&kt=12'
          . '&kd=' . $encoded_url;
      }
      else {
        $href = $link_url;
      }

      $link_count++;

      $linkElement->href = $href;
    }

    $content = $contentHTML->save();
  }

  /**
   * Determine the Silverpop mailing ID that corresponds to a given Drupal
   *  system mailing ID, or vice versa.
   *
   * @param $mail_id
   * @param string $source - e.g. 'drupal' or 'silverpop': Indicates whether 
   *  $mail_id is a Drupal or Silverpop ID.
   * @return string containing the ID of $mail_id's counterpart, or 
   *  FALSE if no relationship for $mail_id has been defined.
   */
  public function drupalSPMailMapping($mail_id = NULL, $source = 'drupal') {
    // If we have cached the mapping array since this object was 
    //  instantiated, use that cached data.
    $cache_id = 'silverpop_mail_id_map_' . $this->instance_id;
    $map = cache_get($cache_id);
    // If not, try to retrieve it from the DB.
    if (!$map) {
      $map = array(
        'drupal' => array(),
        'silverpop' => array(),
      );
      $result = db_query('SELECT * FROM silverpop_transactional_mapping');
      if ($result) {
        while ($row = db_fetch_array($result)) {
          $drupal_mail_key = $row['drupal_mail_key'];
          $sp_mailing_id = $row['sp_mailing_id'];
          if (!empty($drupal_mail_key)) {
            $map['drupal'][$drupal_mail_key] = $sp_mailing_id;
          }
          if (!empty($sp_mailing_id)) {
            $map['silverpop'][$sp_mailing_id] = $drupal_mail_key;
          }
        }
        cache_set($cache_id, $map);
      }
    }
    // If no specific mail ID was provided, return the entire map.
    if (empty($mail_id)) {
      return $map;
    }
    // Otherwise, try to map the specific mail ID to an ID in the other system.
    if (array_key_exists($source, $map)) {
      if (array_key_exists($mail_id, $map[$source])) {
        return $map[$source][$mail_id];
      }
    }
    // If the given mail_id is not in the existing map, add it so that admins
    //  can map it using the Silveprop admin settings form.
    if ($source == 'drupal') {
      $fields = array(
        'drupal_mail_key' => $mail_id,
      );
    }
    elseif ($source == 'silverpop') {
      $fields = array(
        'sp_mailing_id' => $mail_id,
      );
    }
    if (isset($fields)) {
      drupal_write_record('silverpop_transactional_mapping', $fields);
    }

    return FALSE;
  }

  /**
   * Add a recipient to a Silverpop database and/or one or more contact lists
   * @param $columns
   * @param null $contact_lists
   * @param null $list_id
   * @param int $created_from
   * @return bool
   */
  public function addRecipient($columns, $contact_lists = NULL, $list_id = NULL, $created_from = 1) {
    if (empty($list_id)) {
      $list_id = variable_get('silverpop_default_listid', FALSE);
      if (!$list_id) {
        return FALSE;
      }
    }
    $data = array(
      'LIST_ID' => $list_id,
      'CREATED_FROM' => $created_from,
      'UPDATE_IF_FOUND' => 'true',
    );
    if (!empty($contact_lists)) {
      $contact_lists['key_name'] = 'CONTACT_LIST_ID';
      $data['CONTACT_LISTS'] = $contact_lists;
    }
    if (!empty($columns)) {
      $data['COLUMNS'] = $columns;
    }
    return $this->execute('AddRecipient', $data);
  }

  /**
   * Update a contact in an existing Silverpop database.
   * @param $data - an array with keys corresponding to XML node names/values
   */
  public function updateRecipient($data, $list_id = NULL) {
    if (empty($list_id)) {
      $list_id = variable_get('silverpop_default_listid', FALSE);
      if (!$list_id) {
        return FALSE;
      }
    }
    $data['LIST_ID'] = $list_id;
    return $this->execute('UpdateRecipient', $data);
  }

  /**
   * @todo: updateRecipientByUID
   */

  /**
   * Get info about a given Silverpop database, query, or relational table
   * @param null $list_id
   * @return bool
   */
  public function getListMetaData($list_id = NULL) {
    if (empty($list_id)) {
      $list_id = variable_get('silverpop_default_listid', FALSE);
      if (!$list_id) {
        return FALSE;
      }
    }

    $data = array(
      'LIST_ID' => $list_id,
    );

    return $this->execute('GetListMetaData', $data);
  }

  // AddContactToContactList
  public function addContactToContactList($contact_list_id, $contact_id) {
    $data = array(
      'CONTACT_LIST_ID' => $contact_list_id,
      'CONTACT_ID' => $contact_id,
    );

    return $this->execute('AddContactToContactList', $data);
  }

  // RemoveRecipient
  public function removeRecipient($email, $list_id = NULL) {
    if (empty($list_id)) {
      $list_id = variable_get('silverpop_default_listid', FALSE);
      if (!$list_id) {
        return FALSE;
      }
    }

    $data = array(
      'LIST_ID' => $list_id,
      'EMAIL' => $email,
    );

    return $this->execute('RemoveRecipient', $data);
  }

  // RemoveRecipient for non-email-keyed databases that use the Drupal user ID
  //  as their unique identifier (primary key)
  public function removeRecipientByUID($uid, $list_id = NULL) {
    if (empty($list_id)) {
      $list_id = variable_get('silverpop_default_listid', FALSE);
      if (!$list_id) {
        return FALSE;
      }
    }

    $uid_field_name = variable_get('silverpop_primary_key_name', 'DRUPAL_USER_ID');

    $data = array(
      'LIST_ID' => $list_id,
      'COLUMNS' => array(
        $uid_field_name => $uid,
      ),
    );

    return $this->execute('RemoveRecipient', $data);
  }

  // OptOutRecipient for non-email-keyed databases that use the Drupal user ID
  //  as their unique identifier (primary key)
  public function optOutRecipientByUID($uid, $list_id = NULL) {
    if (empty($list_id)) {
      $list_id = variable_get('silverpop_default_listid', FALSE);
      if (!$list_id) {
        return FALSE;
      }
    }

    $uid_field_name = variable_get('silverpop_primary_key_name', 'DRUPAL_USER_ID');

    $data = array(
      'LIST_ID' => $list_id,
      'COLUMNS' => array(
        $uid_field_name => $uid,
      ),
    );

    return $this->execute('OptOutRecipient', $data);
  }

  // SelectRecipientData
  public function selectRecipientData($email, $list_id = NULL, $columns = NULL) {
    if (empty($list_id)) {
      $list_id = variable_get('silverpop_default_listid', FALSE);
      if (!$list_id) {
        return FALSE;
      }
    }

    $data = array(
      'LIST_ID' => $list_id,
      'RETURN_CONTACT_LISTS' => TRUE,
    );
    if (!empty($email)) {
      $data['EMAIL'] = $email;
    }
    if (!empty($columns)) {
      $data['COLUMNS'] = $columns;
    }

    $return = FALSE;

    $response = $this->execute('SelectRecipientData', $data);
    // Extract the RecipientId from the XML returned by the function call.
    if (is_array($response) && count($response) > 1) {
      if ($response[0] == TRUE) {
        $xml = $response[1];

        if (is_object($xml)) {
          $return = array();

          $return['rec_id'] = $xml->getElementsByTagName('RecipientId')->item(0)->nodeValue;
          $contact_lists = $xml->getElementsByTagName('CONTACT_LIST_ID');

          foreach ($contact_lists as $list) {
            $return['contact_lists'][$list->nodeValue] = $list->nodeValue;
          }
        }
      }
    }
    if ($return == FALSE) {
      watchdog('silverpop', t('SelectRecipientData returned an unusable result.'));
    }

    return $return;
  }

  // SelectRecipientDataByUID for non-email-keyed databases that use the Drupal
  //  user ID as their unique identifier (primary key).
  public function selectRecipientDataByUID($uid, $list_id = NULL) {
    $uid_field_name = variable_get('silverpop_primary_key_name', 'DRUPAL_USER_ID');
    $columns = array(
      $uid_field_name => $uid,
    );
    return $this->selectRecipientData(NULL, $list_id, $columns);
  }

  // ScheduleMailing
  public function scheduleMailing($template_id, $list_id, $mailing_name, $mailing_params = array(), $scheduled = NULL, $track_links = TRUE) {
    // Note: the efficiency of retrieving a variable here vs. in the
    //  constructor depends on the number of templates typically used by the
    //  invoking user, and whether the user reuses silverpopAPI objects.
    if (empty($template_id)) {
      $template_id = variable_get('silverpop_default_templateid', FALSE);
    }
    if (empty($list_id)) {
      $list_id = variable_get('silverpop_default_listid', FALSE);
    }
    // @todo: make all of these defaults configurable in the CMS?
    $base_params = array(
      'SEND_HTML' => 'true',
      'SEND_AOL' => 'true',
      'SEND_TEXT' => 'true',
      'VISIBILITY' => 0, // 0 = "Shared"; 1 = "Private."
      'INBOX_MONITOR' => 'true',
      'SEND_TIME_OPTIMIZATION' => 'NONE',
    );
    $data = array_merge($base_params, $mailing_params);
    $data['TEMPLATE_ID'] = $template_id;
    $data['LIST_ID'] = $list_id;
    $data['MAILING_NAME'] = $mailing_name;
    // Make it easier to specify a folder in which to save the sent mailing.
    //  That is, if the invoker has specified a folder name, but has not
    //  specified whether that folder should be created if it doesn't already
    //  exist, assume that he/she would like to create it if necessary.
    if (array_key_exists('PARENT_FOLDER_PATH', $data) && !array_key_exists('CREATE_PARENT_FOLDER', $data)) {
      $data['CREATE_PARENT_FOLDER'] = 'true';
    }

    // Schedule the mailing for a specific delivery time (if not specified,
    //  this mailing will be sent immediately).
    if (!empty($scheduled)) {
      // For maximum convenience, we accept a datetime value in a variety of
      //  formats. The datetime format accepted by the API is:
      //  MM/DD/YYYY HH:MM:SS [AM|PM]
      $acceptable_format = '/\d{1,2}\/\d{1,2}\/\d{1,4} \d{1,2}:\d{1,2}:\d{1,2} (AM|PM)/i';
      // If the scheduled time is already in the right format, go ahead and use
      //  it.
      if (preg_match($acceptable_format, $scheduled)) {
        $data['SCHEDULED'] = $scheduled;
      }
      // Otherwise, try to convert it to the required format.
      else {
        $timestamp = strtotime($scheduled);
        // If we can convert the scheduled time to a timestamp, use that to
        //  create a properly formatted datetime string. If not, we will not
        //  provide any 'SCHEDULED' time, and this mailing will be delivered
        //  immediately.
        if ($timestamp) {
          $data['SCHEDULED'] = date('m/d/Y h:i:s A', $timestamp);
        }
      }
    }
    if (!empty($data['SUBSTITUTIONS'])) {
      // Unless instructed not to do so, configure any hyperlinks within the
      //  substitution set so that Silverpop can track them.
      if ($track_links) {
        foreach ($data['SUBSTITUTIONS'] as $key => $value) {
          $this->enableLinkTracking($data['SUBSTITUTIONS'][$key]);
        }
      }
    }
    $response = $this->execute('ScheduleMailing', $data);
    if (is_array($response) && count($response) > 1) {
      $xml = $response[1];
      $msg_id = $xml->getElementsByTagName('MAILING_ID')->item(0)->nodeValue;
      return $msg_id;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Send an existing mailing to a specified contact.
   * @param $mailing_id
   * @param $recipient_email
   * @param null $columns
   */
  public function sendMailing($mailing_id, $recipient_email, $columns = NULL) {
    $data = array(
      'MailingId' => $mailing_id,
      'RecipientEmail' => $recipient_email,
    );
    if (!empty($columns)) {
      $data['COLUMNS'] = $columns;
    }

    return $this->execute('SendMailing', $data);
  }

  /**
   * Send a transactional email using Silverpop's Engage system, which is not
   *  designed to handle transactional emails.
   *
   * @param $recipient_email
   * @param $uid
   * @param $subject
   * @param $contents
   * @return array
   */
  public function sendEngageTransactionalEmail($recipient_email, $subject, $contents, $message_key = NULL, $columns = array()) {

    $send_result = FALSE;
    
    // If this message does not already contain HTML formatting...
    if (!preg_match('/<.*\w+.*>.*<\/.*\w+>/', $contents)) {

      // ...convert the email body to a format that will make it through the API
      //  transmission and retain some degree of text formatting.
      $spaced_contents = preg_replace('/ /', '&nbsp;', $contents);
      $line_broken_contents = preg_replace('/\\n\\r?/', '<br/>', $spaced_contents);
      $contents = '<div>'
                .   $line_broken_contents
                . '</div>';
    }
    // If the message contains HTML, add link tracking to any links therein, if
    //  link tracking has been enabled for transactional emails.
    else {
      $track_transactional_links = variable_get('silverpop_transactional_track_links', FALSE);
      if ($track_transactional_links) {
        $this->enableLinkTracking($contents);
      }
    }

    // Store the email contents in designated user fields in the SP DB.
    // Each DB field is limited to 3690 characters, so we may need to split
    //  $contents into multiple parts.
    // Use 3500 chars as a cutoff, to be safe.
    $part_length = 3500;
    $parts = str_split($contents, $part_length);
    $max_parts = variable_get('silverpop_transactional_field_count', 4);

    // Plan to build a copy of the columns array that contains blank values for
    //  the trans fields, so we can use it to clear those fields later.
    $trans_columns = array();
//    $reset_trans = array();

    $trans_field_name = variable_get('silverpop_transactional_field_name', 'TRANSACTIONAL_EMAIL_CONTENTS');

    for ($i = 1; $i <= $max_parts; $i++) {
      $field_name = $trans_field_name . '_' . $i;
      if (array_key_exists($i - 1, $parts)) {
        $field_value = $parts[$i - 1];
      }
      else {
        $field_value = '';
      }
      $trans_columns[$field_name] = $field_value;
//      $reset_trans[$field_name] = '';
    }

    $columns = array_merge($columns, $trans_columns);
//    $reset_columns = array_merge($columns, $reset_trans);

    $columns['TRANSACTIONAL_EMAIL_SUBJECT'] = $subject;
//    $reset_columns['TRANSACTIONAL_EMAIL_SUBJECT'] = '';

    $data = array(
      'ALLOW_HTML' => 'true',
      'COLUMNS' => $columns,
    );

    $sp = new silverpopAPI();
    $update_result = $sp->updateRecipient($data);

    if (is_array($update_result) && $update_result[0] == TRUE) {

      // Send a mailing using a special transactional email template, which
      //  reassembles the email contents from the various trans email fields.
      // First, see if this specific email has been linked with its own
      //  Silverpop autoresponder mailing.
      $sp_mailing_id = $this->drupalSPMailMapping($message_key);
      // If not, try to use the default transactional email autoresponder.
      if (!$sp_mailing_id) {
        $sp_mailing_id = variable_get('silverpop_transactional_default_template_id', FALSE);
      }
      if ($sp_mailing_id) {
        $send_result = $this->sendMailing($sp_mailing_id, $recipient_email, $columns);

        // Now clear/reset the user's transactional email fields, to avoid
        //  clutter/confusion.
        // Note: it is possible that clearing the fields here could cause an
        //  apparently successful mailing to be delivered with no contents, if
        //  the sendMailing call is processed but the actual compilation of the
        //  mailing for the given user is delayed until after the fields have
        //  been cleared. Since this is only a feature for a SP admin's benefit,
        //  I am removing it for now to ensure successful delivery
        //  (NRD 2013-06-07).
//        $data = array(
//          'COLUMNS' => $reset_columns,
//        );
//        $update_result = $sp->updateRecipient($data);
//
//        if (!is_array($update_result) || $update_result[0] == FALSE) {
//          watchdog('silverpop', t('Attempt to clear recipient record failed.'));
//        }
      }
      else {
        $error_string = t('Silverpop has been designated as a '
          . 'transactional email provider, but no mailing ID has been set.');
        watchdog('silverpop', $error_string, array(), WATCHDOG_ERROR);
      }
    }
    else {
      watchdog('silverpop', t('Attempt to update recipient record failed.'));
    }

    return $send_result;
  }

  /**
   * Send a transactional email using Silverpop's Engage system, which is not
   *  designed to handle transactional emails. This is a wrapper method for
   *  sendEngageTransactionalEmail that allows the invoker to designate the
   *  recipient by the recipient's Drupal user ID, rather than email address.
   *
   * @param $recipient_email
   * @param $subject
   * @param $contents
   * @param $uid
   * @return array
   */
  public function sendEngageTransactionalEmailByUID($recipient_email, $subject, $contents, $uid, $message_key = NULL) {

    $uid_field_name = variable_get('silverpop_primary_key_name', 'DRUPAL_USER_ID');

    $columns = array(
      $uid_field_name => $uid,
    );

    return $this->sendEngageTransactionalEmail($recipient_email, $subject, $contents, $message_key, $columns);
  }

  /**
   * Initiate an export of mailing data. The data will be stored in a
   *  character-delimited file (whose format is negotiable via the
   *  'EXPORT_FORMAT' param),
   *  archived/zipped, and then stored either in the Engage "Stored Files" area
   *  or in the "download" directory in your FTP repository (if directed via the
   *  'MOVE_TO_FTP' param).
   */
  public function rawRecipientDataExport($data = array()) {
    return $this->execute('RawRecipientDataExport', $data);
  }
}