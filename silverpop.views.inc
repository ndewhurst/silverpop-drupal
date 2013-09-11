<?php

/**
* Implements hook_views_data_alter.
* @param $data
* @return mixed
*/
//function silverpop_views_data_alter(&$data) {
//  if (module_exists('date')) {
//    // Tell Views to treat this numeric timestamp field as a date.
//    $data['silverpop_report_data']['event_timestamp']['field']['handler'] = 'views_handler_field_date';
//    $data['silverpop_report_data']['event_timestamp']['filter']['handler'] = 'views_handler_filter_date';
//    $data['silverpop_report_data']['event_timestamp']['argument']['handler'] = 'views_handler_argument_date';
//  }
//}

/**
 * Implementation of hook_views_data().
 */
function silverpop_views_data() {
  $data = array();
  $data['silverpop_report_data']['table']['group'] = t('Silverpop');

  $data['silverpop_report_data']['table']['base'] = array(
    'field' => 'record_id',
    'title' => t('Silverpop Report Data'),
    'help' => t('Records of events related to transactional mailings'),
    'weight' => -10,
  );

//  $data['silverpop_report_data']['record_id'] = array(
//    'title' => t('Record ID'),
//  );

  $data['silverpop_report_data']['sp_mailing_name'] = array(
    'title' => t('Silverpop Mailing Name'),
    'help' => t('The name of a Silverpop mailing (e.g. an autoresponder) associated with an event'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['drupal_mail_key'] = array(
    'title' => t('Drupal Mail Key'),
    'help' => t('The machine name of the Drupal message that initiated a Silverpop mailing'),
    'field' => array(
      'click sortable' => TRUE,
      'handler' => 'silverpop_mail_logger_mailkey_link',
    ),
  );

  $data['silverpop_report_data']['sp_mailing_id'] = array(
    'title' => t('Silverpop Mailing ID'),
    'help' => t('The numeric ID of a Silverpop mailing (e.g. an autoresponder) associated with an event'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['recipient_email'] = array(
    'title' => t('Recipient Email'),
    'help' => t('The email address of the recipient associated with an event'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['recipient_uid'] = array(
    'title' => t("Recipient's Drupal UID"),
    'help' => t('The Drupal user ID of the recipient associated with an event'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['event_type'] = array(
    'title' => t('Event Type'),
    'help' => t('The name of the Silverpop event type (e.g. "Send," "Open," or "Hard Bounce")'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['event_timestamp'] = array(
    'title' => t('Event Date/Time'),
    'help' => t('Event timestamp'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
//      'handler' => 'silverpop_handler_filter_easy_date',
      'handler' => 'views_handler_filter_date',
    ),
    'argument' => array(
      'handler' => 'views_argument_filter_date',
    ),
  );

  $data['silverpop_report_data']['sp_recipient_id'] = array(
    'title' => t('Silverpop Recipient ID'),
    'help' => t('The recipient\'s unique, numeric ID in the Silverpop database'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['recipient_type'] = array(
    'title' => t('Recipient Type'),
    'help' => t('The type of recipient, according to Silverpop (e.g. "Normal")'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['sp_report_id'] = array(
    'title' => t('Silverpop Report ID'),
    'help' => t('The numeric ID of the Silverpop report containing the event'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['content_id'] = array(
    'title' => t('Content ID'),
    'help' => t('Content ID'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['click_name'] = array(
    'title' => t('Click Name'),
    'help' => t('The name of a link that is the subject of a click event'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  $data['silverpop_report_data']['click_url'] = array(
    'title' => t('Click URL'),
    'help' => t('The destination URL of a link that is the subject of a click event'),
    'field' => array(
      'click sortable' => TRUE,
    ),
  );

  return $data;
}

/**
 * Implementation of hook_views_handlers() to register all of the basic handlers
 * views uses.
 */
function silverpop_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'silverpop') . '/views',
    ),
    'handlers' => array(
      // The name of my handler
      'silverpop_handler_filter_easy_date' => array(
        // The name of the handler we are extending.
        'parent' => 'views_handler_filter_numeric',
      ),
      'silverpop_mail_logger_mailkey_link' => array(
        'parent' => 'views_handler_filter_string',
      ),
    ),
  );
}

//  $timestamp = $data->silverpop_report_data_event_timestamp;
//  $drupal_mail_key = $data->silverpop_report_data_drupal_mail_key;
//  $recipient_email = $data->silverpop_report_data_recipient_email;
//
//  $sql = 'SELECT mlid FROM mail_logger WHERE mailkey = %s AND to = %s AND ABS(date_sent - %d) < 10 ORDER BY ABS(date_sent - %d) DESC LIMIT 1';
//  $query_params = array(
//    $drupal_mail_key,
//    $recipient_email,
//    $timestamp,
//  );
//  $mlid = db_result(db_query($sql, $query_params));
//
//  if ($mlid) {
//    print l($data->silverpop_report_data_drupal_mail_key, 'admin/reports/mail-logger/mail/' . $mlid);
//  }
//  else {
//    print $data->silverpop_report_data_drupal_mail_key;
//  }