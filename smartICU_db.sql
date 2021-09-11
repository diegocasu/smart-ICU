CREATE DATABASE  IF NOT EXISTS `smartICU` /*!40100 DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci */;
USE `smartICU`;
-- MySQL dump 10.13  Distrib 5.7.35, for Linux (x86_64)
--
-- Host: 127.0.0.1    Database: smartICU
-- ------------------------------------------------------
-- Server version	5.7.35-0ubuntu0.18.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `blood_pressure`
--

DROP TABLE IF EXISTS `blood_pressure`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `blood_pressure` (
  `sample_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `sample` float NOT NULL,
  `unit` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `timestamp` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `patient_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `monitor_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`sample_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `blood_pressure`
--

LOCK TABLES `blood_pressure` WRITE;
/*!40000 ALTER TABLE `blood_pressure` DISABLE KEYS */;
/*!40000 ALTER TABLE `blood_pressure` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `heart_rate`
--

DROP TABLE IF EXISTS `heart_rate`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `heart_rate` (
  `sample_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `sample` float NOT NULL,
  `unit` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `timestamp` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `patient_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `monitor_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`sample_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `heart_rate`
--

LOCK TABLES `heart_rate` WRITE;
/*!40000 ALTER TABLE `heart_rate` DISABLE KEYS */;
/*!40000 ALTER TABLE `heart_rate` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `oxygen_saturation`
--

DROP TABLE IF EXISTS `oxygen_saturation`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `oxygen_saturation` (
  `sample_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `sample` float NOT NULL,
  `unit` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `timestamp` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `patient_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `monitor_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`sample_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `oxygen_saturation`
--

LOCK TABLES `oxygen_saturation` WRITE;
/*!40000 ALTER TABLE `oxygen_saturation` DISABLE KEYS */;
/*!40000 ALTER TABLE `oxygen_saturation` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `respiration`
--

DROP TABLE IF EXISTS `respiration`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `respiration` (
  `sample_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `sample` float NOT NULL,
  `unit` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `timestamp` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `patient_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `monitor_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`sample_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `respiration`
--

LOCK TABLES `respiration` WRITE;
/*!40000 ALTER TABLE `respiration` DISABLE KEYS */;
/*!40000 ALTER TABLE `respiration` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `temperature`
--

DROP TABLE IF EXISTS `temperature`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `temperature` (
  `sample_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `sample` float NOT NULL,
  `unit` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `timestamp` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `patient_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `monitor_id` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`sample_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `temperature`
--

LOCK TABLES `temperature` WRITE;
/*!40000 ALTER TABLE `temperature` DISABLE KEYS */;
/*!40000 ALTER TABLE `temperature` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2021-09-11 16:38:30
