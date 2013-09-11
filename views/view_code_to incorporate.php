<?php
/**********
PHP code for rendering the "drupal_mail_key" field as a link to a mail_logger entry for the given mailing, if one exists:
**********/


  $timestamp = $data->silverpop_report_data_event_timestamp;
  $drupal_mail_key = $data->silverpop_report_data_drupal_mail_key;
  $recipient_email = $data->silverpop_report_data_recipient_email;

$sql = 'SELECT mlid FROM mail_logger ml WHERE ml.to = "%s" AND ml.mailkey = "%s" AND ABS(CAST(ml.date_sent as SIGNED) - CAST(%d as SIGNED)) < 180 ORDER BY ABS(CAST(ml.date_sent as SIGNED) - CAST(%d as SIGNED)) ASC LIMIT 1';
      $query_params = array(
        $recipient_email,
        $drupal_mail_key,
        $timestamp,
        $timestamp,
      );
      $query = db_query($sql, $query_params);
      $mlid = db_result($query);

  if ($mlid) {
    print l($data->silverpop_report_data_drupal_mail_key, 'admin/reports/mail-logger/mail/' . $mlid);
  }
  else {
    print $data->silverpop_report_data_drupal_mail_key;
  }





/*************
Full Export Code for View
**************/
$view = new view;
$view->name = 'silverpop_reporting_dashboard';
$view->description = 'silverpop_report_data';
$view->tag = 'tw';
$view->view_php = '';
$view->base_table = 'silverpop_report_data';
$view->is_cacheable = FALSE;
$view->api_version = 2;
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
$handler = $view->new_display('default', 'silverpop_reporting_dashboard', 'default');
$handler->override_option('fields', array(
  'event_timestamp' => array(
    'label' => 'Event Date/Time',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'date_format' => 'small',
    'custom_date_format' => '',
    'exclude' => 0,
    'id' => 'event_timestamp',
    'table' => 'silverpop_report_data',
    'field' => 'event_timestamp',
    'relationship' => 'none',
  ),
  'sp_mailing_id_1' => array(
    'label' => 'sp_mailing_id',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => '',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 1,
    'id' => 'sp_mailing_id_1',
    'table' => 'silverpop_report_data',
    'field' => 'sp_mailing_id',
    'relationship' => 'none',
  ),
  'sp_mailing_name' => array(
    'label' => 'Silverpop Mailing Name',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 1,
      'path' => 'https://engage1.silverpop.com/mailingsSummary.do?action=mailingSummary&mailingId=[sp_mailing_id_1]',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'sp_mailing_name',
    'table' => 'silverpop_report_data',
    'field' => 'sp_mailing_name',
    'relationship' => 'none',
  ),
  'drupal_mail_key' => array(
    'label' => 'Drupal Mailing Name',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 1,
    'id' => 'drupal_mail_key',
    'table' => 'silverpop_report_data',
    'field' => 'drupal_mail_key',
    'relationship' => 'none',
    'override' => array(
      'button' => 'Override',
    ),
  ),
  'phpcode' => array(
    'label' => 'Drupal Mailing Name',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'value' => '<?php
  $timestamp = $data->silverpop_report_data_event_timestamp;
  $drupal_mail_key = $data->silverpop_report_data_drupal_mail_key;
  $recipient_email = $data->silverpop_report_data_recipient_email;

$sql = \'SELECT mlid FROM mail_logger ml WHERE ml.to = "%s" AND ml.mailkey = "%s" AND ABS(CAST(ml.date_sent as SIGNED) - CAST(%d as SIGNED)) < 180 ORDER BY ABS(CAST(ml.date_sent as SIGNED) - CAST(%d as SIGNED)) ASC LIMIT 1\';
      $query_params = array(
        $recipient_email,
        $drupal_mail_key,
        $timestamp,
        $timestamp,
      );
      $query = db_query($sql, $query_params);
      $mlid = db_result($query);

  if ($mlid) {
    print l($data->silverpop_report_data_drupal_mail_key, \'admin/reports/mail-logger/mail/\' . $mlid);
  }
  else {
    print $data->silverpop_report_data_drupal_mail_key;
  }
?>',
    'exclude' => 0,
    'id' => 'phpcode',
    'table' => 'customfield',
    'field' => 'phpcode',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'recipient_email' => array(
    'label' => 'Recipient Email',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'recipient_email',
    'table' => 'silverpop_report_data',
    'field' => 'recipient_email',
    'relationship' => 'none',
  ),
  'recipient_uid' => array(
    'label' => 'Recipient\'s Drupal User ID',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 1,
      'path' => 'user/[recipient_uid]',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => '',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 0,
    'id' => 'recipient_uid',
    'table' => 'silverpop_report_data',
    'field' => 'recipient_uid',
    'relationship' => 'none',
  ),
  'event_type' => array(
    'label' => 'Event Type',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'event_type',
    'table' => 'silverpop_report_data',
    'field' => 'event_type',
    'relationship' => 'none',
  ),
  'sp_mailing_id' => array(
    'label' => 'Silverpop Mailing ID',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 1,
      'path' => 'https://engage1.silverpop.com/reportsAutoresponders.do?mailingId=[sp_mailing_id]&isCampaign=0&reportId=0&action=reportsAutoresponders',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => '',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 0,
    'id' => 'sp_mailing_id',
    'table' => 'silverpop_report_data',
    'field' => 'sp_mailing_id',
    'relationship' => 'none',
  ),
  'sp_report_id' => array(
    'label' => 'Silverpop Report ID',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 1,
      'path' => 'https://engage1.silverpop.com/reportsSummary.do?mailingId=[sp_mailing_id]&originatingReport=autoresponders&reportId=[sp_report_id]&action=reportsSummary',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => '',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 0,
    'id' => 'sp_report_id',
    'table' => 'silverpop_report_data',
    'field' => 'sp_report_id',
    'relationship' => 'none',
  ),
  'recipient_type' => array(
    'label' => 'Silverpop Recipient Type',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'recipient_type',
    'table' => 'silverpop_report_data',
    'field' => 'recipient_type',
    'relationship' => 'none',
  ),
  'content_id' => array(
    'label' => 'Silverpop Content ID',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'content_id',
    'table' => 'silverpop_report_data',
    'field' => 'content_id',
    'relationship' => 'none',
  ),
  'click_name' => array(
    'label' => 'Clicked Link Name',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'click_name',
    'table' => 'silverpop_report_data',
    'field' => 'click_name',
    'relationship' => 'none',
  ),
  'click_url' => array(
    'label' => 'Clicked Link URL',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
    ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'click_url',
    'table' => 'silverpop_report_data',
    'field' => 'click_url',
    'relationship' => 'none',
  ),
));
$handler->override_option('filters', array(
  'event_timestamp' => array(
    'operator' => 'between',
    'value' => array(
      'type' => 'date',
      'value' => '',
      'min' => '',
      'max' => '',
    ),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 0,
      'operator' => 'event_timestamp_op',
      'identifier' => 'event_timestamp',
      'label' => 'Event Date/Time',
      'optional' => 1,
      'remember' => 0,
    ),
    'id' => 'event_timestamp',
    'table' => 'silverpop_report_data',
    'field' => 'event_timestamp',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'event_type' => array(
    'operator' => '=',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'event_type_op',
      'identifier' => 'event_type',
      'label' => 'Event Type',
      'optional' => 1,
      'remember' => 0,
    ),
    'case' => 1,
    'id' => 'event_type',
    'table' => 'silverpop_report_data',
    'field' => 'event_type',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'recipient_email' => array(
    'operator' => '=',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'recipient_email_op',
      'identifier' => 'recipient_email',
      'label' => 'Recipient Email',
      'optional' => 1,
      'remember' => 0,
    ),
    'case' => 1,
    'id' => 'recipient_email',
    'table' => 'silverpop_report_data',
    'field' => 'recipient_email',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'recipient_uid' => array(
    'operator' => '=',
    'value' => array(
      'value' => '',
      'min' => '',
      'max' => '',
    ),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'recipient_uid_op',
      'identifier' => 'recipient_uid',
      'label' => 'Recipient\'s Drupal User ID',
      'optional' => 1,
      'remember' => 0,
    ),
    'id' => 'recipient_uid',
    'table' => 'silverpop_report_data',
    'field' => 'recipient_uid',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'drupal_mail_key' => array(
    'operator' => '=',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'drupal_mail_key_op',
      'identifier' => 'drupal_mail_key',
      'label' => 'Drupal Mailing Name',
      'optional' => 1,
      'remember' => 0,
    ),
    'case' => 1,
    'id' => 'drupal_mail_key',
    'table' => 'silverpop_report_data',
    'field' => 'drupal_mail_key',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'sp_mailing_name' => array(
    'operator' => '=',
    'value' => '',
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'sp_mailing_name_op',
      'identifier' => 'sp_mailing_name',
      'label' => 'Silverpop Mailing Name',
      'optional' => 1,
      'remember' => 0,
    ),
    'case' => 1,
    'id' => 'sp_mailing_name',
    'table' => 'silverpop_report_data',
    'field' => 'sp_mailing_name',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'sp_mailing_id' => array(
    'operator' => '=',
    'value' => array(
      'value' => '',
      'min' => '',
      'max' => '',
    ),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'sp_mailing_id_op',
      'identifier' => 'sp_mailing_id',
      'label' => 'Silverpop Mailing ID',
      'optional' => 1,
      'remember' => 0,
    ),
    'id' => 'sp_mailing_id',
    'table' => 'silverpop_report_data',
    'field' => 'sp_mailing_id',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'sp_report_id' => array(
    'operator' => '=',
    'value' => array(
      'value' => '',
      'min' => '',
      'max' => '',
    ),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'sp_report_id_op',
      'identifier' => 'sp_report_id',
      'label' => 'Silverpop Report ID',
      'optional' => 1,
      'remember' => 0,
    ),
    'id' => 'sp_report_id',
    'table' => 'silverpop_report_data',
    'field' => 'sp_report_id',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
  'sp_recipient_id' => array(
    'operator' => '=',
    'value' => array(
      'value' => '',
      'min' => '',
      'max' => '',
    ),
    'group' => '0',
    'exposed' => TRUE,
    'expose' => array(
      'use_operator' => 1,
      'operator' => 'sp_recipient_id_op',
      'identifier' => 'sp_recipient_id',
      'label' => 'Silverpop Recipient ID',
      'optional' => 1,
      'remember' => 0,
    ),
    'id' => 'sp_recipient_id',
    'table' => 'silverpop_report_data',
    'field' => 'sp_recipient_id',
    'override' => array(
      'button' => 'Override',
    ),
    'relationship' => 'none',
  ),
));
$handler->override_option('access', array(
  'type' => 'perm',
  'perm' => 'table wizard administration',
));
$handler->override_option('cache', array(
  'type' => 'none',
));
$handler->override_option('title', 'Silverpop Transactional Email Reporting Dashboard');
$handler->override_option('header', 'This dashboard displays one record for each event that has been registered in your Silveprop account and retrieved by the Silverpop Drupal module. Event records are retrieved whenever cron runs on your site. To retrieve the most up-to-date data possible, you can use the form at the bottom of this page.
You can click on most of the column headers to sort the table by that field. What\'s more, you can filter the result set using the one or more of the criteria in the fields below. To specify a date range, enter a start date and/or an end date in the first and second text fields beneath the "Event Date/Time" label, respectively. You will need to format the date in this way: "YYYY-MM-DD." You can optionally add a time, such as "2013-08-31 13:00." If no time is specified, the filter will search for events using midnight on the specified date as the cutoff time.');
$handler->override_option('header_format', '1');
$handler->override_option('header_empty', 1);
$handler->override_option('footer_format', '3');
$handler->override_option('footer_empty', 0);
$handler->override_option('empty', 'There are no rows in this table.');
$handler->override_option('empty_format', '1');
$handler->override_option('items_per_page', 25);
$handler->override_option('use_pager', '1');
$handler->override_option('style_plugin', 'table');
$handler->override_option('style_options', array(
  'grouping' => '',
  'override' => 1,
  'sticky' => 1,
  'order' => 'desc',
  'columns' => array(
    'record_id' => 'record_id',
    'sp_mailing_name' => 'sp_mailing_name',
    'drupal_mail_key' => 'drupal_mail_key',
    'sp_mailing_id' => 'sp_mailing_id',
    'recipient_email' => 'recipient_email',
    'recipient_uid' => 'recipient_uid',
    'event_type' => 'event_type',
    'event_timestamp' => 'event_timestamp',
    'sp_recipient_id' => 'sp_recipient_id',
    'recipient_type' => 'recipient_type',
    'sp_report_id' => 'sp_report_id',
    'content_id' => 'content_id',
    'click_name' => 'click_name',
    'click_url' => 'click_url',
  ),
  'info' => array(
    'record_id' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'sp_mailing_name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'drupal_mail_key' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'sp_mailing_id' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'recipient_email' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'recipient_uid' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'event_type' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'event_timestamp' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'sp_recipient_id' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'recipient_type' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'sp_report_id' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'content_id' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'click_name' => array(
      'sortable' => 1,
      'separator' => '',
    ),
    'click_url' => array(
      'sortable' => 1,
      'separator' => '',
    ),
  ),
  'default' => 'event_timestamp',
));
$handler = $view->new_display('page', 'Page', 'page_1');
$handler->override_option('header', '<p>
This dashboard displays one record for each event that has been registered in your Silveprop account and retrieved by the Silverpop Drupal module. Event records are retrieved whenever cron runs on your site. To retrieve the most up-to-date data possible, you can use the "Retrieve more data from Silverpop" form below.
</p>
<p>
You can click on most of the column headers to sort the table by that field. What\'s more, you can filter the result set using the one or more of the criteria in the fields below. To specify a date range, enter a start date and/or an end date in the first and second text fields beneath the "Event Date/Time" label, respectively. You will need to format the date in this way: "YYYY-MM-DD." You can optionally add a time, such as "2013-08-31 13:00." If no time is specified, the filter will search for events using midnight on the specified date as the cutoff time.
</p>
<div style="background-color: rgb(230, 230, 230); padding: 1em;">
<?php
  $block_content_string = \'You can use the button below to request a \'
            . \'new \'
            . \'data export from your Silverpop database. When the Silverpop \'
            . \'data job is finished, the exported data will be available for \'
            . \'retrieval. Clicking this button will initiate a new set of \'
            . \'exports and then retrieve any available data and import it \'
            . \'into Drupal. Because data \'
            . \'jobs can take some time to complete, you may need to perform \'
            . \'this export/fetch twice to get the latest possible data.\';
          $block_contents = \'<p>\' . t($block_content_string) . \'</p>\'
            . drupal_get_form(\'silverpop_reports_dashboard_form\');
          $block = \'<h3>\' . t(\'Retrieve more data from Silverpop\') . \'</h3>\'
               . $block_contents;
          
          print $block;
?>
</div>
<h3 style="margin-top: 1em;">Filters:</h3>');
$handler->override_option('header_format', '3');
$handler->override_option('path', 'admin/reports/silverpop-transactional');
$handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => '0',
  'name' => 'navigation',
));
$handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
));
