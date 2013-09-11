<?php

// Copyright @badzillacouk http://www.badzilla.co.uk
// Licence GPL. This program may be distributed as per the terms of GPL and all credits
// must be retained
//
// If you find this script useful, please consider a donation to help me fund my web presence
// and encourage me to develop more products to be placed under the terms of GPL
// To donate, go to http://www.badzilla.co.uk and click on the donation button
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
class silverpopXML {

  private $wrapper;
  private $login_required;
  private $endpoint;
  private $userid;
  private $password;
  private $sessionid;

  // Silverpop outgoing API call
  // Pass a @body DOMDocument without the Envelope/Body nodes
  // And a TRUE / FALSE depending upon whether a jsession is requred or not
  public function __construct($body, $login_required = FALSE) {
    $silverpop_endpoint = variable_get('silverpop_endpoint', FALSE);
    if (empty($silverpop_endpoint)) {
      watchdog('silverpop', 'Silverpop endpoint (URL) is not set');
      return array(FALSE);
    }
    else {
      $this->endpoint = $silverpop_endpoint;
    }

    if ($login_required) {
      $silverpop_userid = variable_get('silverpop_userid', FALSE);
      if (empty($silverpop_userid)) {
        watchdog('silverpop', 'Silverpop user ID is required but not set');
        return array(FALSE);
      }
      else {
        $this->userid = $silverpop_userid;
      }

      $silverpop_password = variable_get('silverpop_password', FALSE);
      if (empty($silverpop_password)) {
        watchdog('silverpop', 'Silverpop password is required but not set');
        return array(FALSE);
      }
      else {
        $this->password = $silverpop_password;
      }
    }

    // Create the outgoing xml.
    $this->wrapper = $this->_build_envelope($body);
    $this->login_required = $login_required;
  }

  // Top level API Call - all calling functions must go through this and not the lower-level function
  // Returns an array: First value FALSE or TRUE on success, and second value the DOMDocument response
  // (if converting the response to XML was possible)
  public function apicall() {

    $ret = array();

    if ($this->login_required)
      if (!$this->_login())
        return $ret;

    $ret = $this->_do_call();

    if ($this->login_required)
      $this->_logout();

    return $ret;
  }

  // build the login call to get the session id
  private function _login() {

    $dom = new DOMDocument;
    $user_text = $dom->createTextNode($this->userid);
    $user = $dom->createElement('USERNAME');

    $password_text = $dom->createTextNode($this->password);
    $password = $dom->createElement('PASSWORD');

    $method = $dom->createElement('Login');

    $user->appendChild($user_text);
    $password->appendChild($password_text);
    $method->appendChild($user);
    $method->appendChild($password);
    $dom->appendChild($method);

    list($retcode, $response) = $this->_do_call($this->_build_envelope($dom));

    // extract the sessionid if it is available
    if (!$retcode)
      return NULL;

    if (is_object($response) and isset($response->getElementsByTagName('SESSIONID')->item(0)->nodeValue)) {
      $this->sessionid = $response->getElementsByTagName('SESSIONID')->item(0)->nodeValue;
      return TRUE;
    }

    return NULL;
  }

  // build the logout call
  private function _logout() {

    $dom = new DOMDocument;
    $method = $dom->createElement('Logout');
    $dom->appendChild($method);

    $this->_do_call($this->_build_envelope($dom));

    return TRUE;
  }

  // build the xml envelope
  // Pass the inner method and paramters as a DOMDocument
  // Return the stringified xml
  private function _build_envelope($inner) {

    // Create the outgoing xml.
    // First create the outer wrapper
    $wrapper = new DOMDocument;
    $envelope = $wrapper->createElement('Envelope');
    $body_tag = $wrapper->createElement('Body');

    $wrapper->appendChild($envelope);
    $envelope->appendChild($body_tag);

    // Now insert the passed parameters into this wrapper
    $node = $inner->documentElement;
    $node = $wrapper->importNode($node, true);
    $wrapper->getElementsByTagName('Body')->item(0)->appendChild($node);

    // Output the full XML to an external file for diagnostic purposes.
    //  @todo: make this a configurable option via CMS.
//    global $sp_xml_filename_count;
//    if (empty($sp_xml_filename_count)) {
//      $sp_xml_filename_count = 0;
//    }
//    $filename = date('Ymd_His') . '_' . $sp_xml_filename_count . '_Silverpop_XML_submission.xml';
//    $uri = '../temp/' . $filename;
//    $fp = fopen($uri, 'w+');
//    fwrite($fp, $wrapper->saveXML());
//    fclose($fp);
//    $sp_xml_filename_count++;

    return $wrapper->saveXML();
  }

  // low level API call not available to clients
  private function _do_call($wrapper = NULL) {

    // ok now ready to go
    if (!$wrapper)
      $data = "xml=" . urlencode($this->wrapper);
    else
      $data = "xml=" . urlencode($wrapper);

    // open connection
    // port 80, timeout of 20
    if (($sock = @fsockopen($this->endpoint, 80, $errno, $errstr, 20)) == NULL) {
      watchdog('silverpop', 'Could not connect to host with error number: @errno and diagnostic: @errstr',
        array('@errno' => $errno, '@errstr' => $errstr), WATCHDOG_ERROR);
      return array();
    }

    // outgoing message
    $jsessionid = '';
    if ($this->login_required)
      $jsessionid = ';jsessionid=' . $this->sessionid;
    fputs($sock, "POST /servlet/XMLAPI{$jsessionid} HTTP/1.1\n");
    fputs($sock, "Host: " . $this->endpoint . "\n");
    fputs($sock, "Content-type: application/x-www-form-urlencoded\n");
    fputs($sock, "Content-length: " . strlen ($data) . "\n");
    fputs($sock, "Connection: close\n\n");
    fputs($sock, $data);

    // incoming
    $buffer = "";
    while (!feof($sock))
      $buffer .= fgets($sock);

    fclose($sock);

    // Ok so was there an error? If so, stick it in the log
    if (($pos = stripos($buffer, '<Envelope>')) === NULL) {
      watchdog('silverpop', 'Unspecified error diagnostic. Output dump: @dump', array('@dump' => $buffer), WATCHDOG_ERROR);
      return array(NULL);
    } else {
      $epos = stripos($buffer, '</Envelope>') + strlen('</Envelope>');
      $b_body = substr($buffer, $pos, $epos - $pos);

      $response = new DOMDocument;
      $response->loadXML($b_body);

      $success = $response->getElementsByTagName('SUCCESS');
      $fault = $response->getElementsByTagName('FaultString');
      $errorid = $response->getElementsByTagName('errorid');

      if ($success->length and strtoupper($response->getElementsByTagName('SUCCESS')->item(0)->nodeValue) == 'FALSE') {
        $err_arr = array();
        $msg = 'API call failed.';

        // did we get an error message?
        if ($fault->length) {
          $msg .= ' Diagnostic: @fault';
          $err_arr['@fault'] = $response->getElementsByTagName('FaultString')->item(0)->nodeValue;
        }

        // did we get an error number?
        if ($errorid->length) {
          $msg .= ' Error code: @errorid';
          $err_arr['@errorid'] = $response->getElementsByTagName('errorid')->item(0)->nodeValue;
        }

        watchdog('silverpop', $msg, $err_arr, WATCHDOG_ERROR);
        return array(FALSE, $response);
      }
    }

    return array(TRUE, $response);
  }
}

